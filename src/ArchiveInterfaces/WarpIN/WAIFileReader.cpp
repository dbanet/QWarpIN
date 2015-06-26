#include "WAIFileReader.h"

WAIFileReader::WAIFileReader(WFileSystemNode *fsNode, qint64 bufferSize):
	fsNode(fsNode),
	bufferSize(bufferSize){
	this->arcFile=((WarpINArchiveInterface*)this->fsNode->file()->property("archive").value<void*>())->arcFile();
	this->inputBuffer=(char*)malloc(this->bufferSize);
	this->outputBuffer=(char*)malloc(this->bufferSize);
	this->seek(0);
}

/**
 * @brief Decompresses maxlen (or until the file's end) bytes of the
 *        compressed file to the first input parameter char *data.
 *
 * First outputs this->outputBuffer since this->bufCur til its end or
 * until maxlen is reached, then, if maxlen is not reached, decompresses
 * a new portion of data to this->outputBuffer, and outputs it again.
 *
 * @returns The amount of decompressed bytes output to char *data.
 */
qint64 WAIFileReader::read(char *data,qint64 maxlen){
	qint64 savedPos=arcFile->pos();
	// bz2 at this moment has already decompressed some data, because this->seek
	// has been already performed, at least once. So to maintain the relevant
	// information about the amounts of data decompressed at each very call of this
	// function, a special variable qint64 bz2bytesReadErr is used. Every time
	// a necessity to calculate the number of bytes decompressed by bz2 since the
	// call of this function arises, the total number of bytes decompressed by bz2,
	// which is calculated by the following formulae:
	//     (qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);
	// must by decreased by qint64 bz2bytesReadErr.
	qint64 bz2bytesReadErr=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);

	qint64 bytesLeftToCopy,bytesToBeCopied;
	bytesToBeCopied=(maxlen<this->fsNode->file()->size()-this->decompCur)?
				maxlen:
				this->fsNode->file()->size()-this->decompCur;
	bytesLeftToCopy=bytesToBeCopied;

	// there is some data in this->outputBuffer initially, so dealing with it
	qint64 bytesInBufferInitially=this->bytesBuffered-this->bufCur;
	if(bytesLeftToCopy<=bytesInBufferInitially){ // got enough data in this->outputBuffer
		memcpy(data,this->outputBuffer+this->bufCur,bytesLeftToCopy); // so return
		this->bufCur+=bytesLeftToCopy;
		this->decompCur+=bytesLeftToCopy;
		return bytesLeftToCopy;
	}
	else{ // got not enough data in this->outputBuffer, so just copy it, and decompress some more
		memcpy(data,this->outputBuffer+this->bufCur,bytesInBufferInitially);
		bytesLeftToCopy-=bytesInBufferInitially;
		this->bufCur+=bytesLeftToCopy;
		this->decompCur+=bytesLeftToCopy;
	}

	forever{
		// okay, so decompressing more data
		// the data in the buffers is no longer needed, so purging them for the later use
		memset(this->inputBuffer,0,this->bufferSize);
		memset(this->outputBuffer,0,this->bufferSize);
		this->z->next_in=this->inputBuffer;
		this->z->next_out=this->outputBuffer;
		this->z->avail_out=this->bufferSize;

		// reading this->bufferSize bytes
		qint64 bytesRead=-1;
		if(!~(bytesRead=arcFile->read(this->inputBuffer,this->bufferSize)))
			throw E_WPIAI_FileReadError();
		this->arcCur+=bytesRead;
		this->compCur+=bytesRead;
		this->z->next_in=this->inputBuffer;

		// handling errors (reached the end of the archive)
		if(bytesRead<this->bufferSize)
			;

		// read more than needed: ran out of the compressed file data (overread compCur-compSize bytes)
		if(this->compCur>this->fsNode->file()->property("arcCompSize").toLongLong()){
			// calculating bytes of compressed data left available (til the end of it)
			this->z->avail_in=this->fsNode->file()->property("arcCompSize").toLongLong()-(this->compCur-bytesRead);

			// unrolling pos to the end of the comp file
			this->arcCur-=this->compCur-this->fsNode->file()->property("arcCompSize").toLongLong();
			this->compCur-=this->compCur-this->fsNode->file()->property("arcCompSize").toLongLong();
			arcFile->seek(this->arcCur);
		}
		// all fine, got this->bufferSize of compressed data
		else
			this->z->avail_in=this->bufferSize;

		// decompressing the whole this->inputBuffer
		forever{
			int rc=BZ2_bzDecompress(this->z);

			// an unrecoverable decompression error has occured
			if(BZ_PARAM_ERROR      == rc
			|| BZ_DATA_ERROR       == rc
			|| BZ_DATA_ERROR_MAGIC == rc
			|| BZ_MEM_ERROR        == rc
			) throw E_WPIAI_FileDecompressionError();

			// compressed file's end has been reached (all data has been decompressed and is now in this->outputBuffer)
			if(BZ_STREAM_END==rc){
				// so just outputting bytesLeftToCopy or all of it, and then returning
				qint64 bytesAlreadyCopied=bytesToBeCopied-bytesLeftToCopy;
				qint64 bytesDecompressedAll=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32)-bz2bytesReadErr;
				qint64 bytesJustDecompressed=bytesDecompressedAll-(bytesAlreadyCopied-bytesInBufferInitially);
				this->bytesBuffered=bytesJustDecompressed;
				qint64 bytesToCopyNow=(bytesLeftToCopy<bytesJustDecompressed)?
				                      	bytesLeftToCopy:
				                      	bytesJustDecompressed;
				this->decompCur+=bytesToCopyNow;
				this->bufCur=bytesToCopyNow;
				memcpy(data+bytesAlreadyCopied,this->outputBuffer,bytesToCopyNow);
				bytesLeftToCopy-=bytesToCopyNow;
				this->arcFile->seek(savedPos);
				return bytesToBeCopied;
			}

			// the output buffer has ended
			if(0==this->z->avail_out
			&& 0< this->z->avail_in){
				qint64 bytesAlreadyCopied=bytesToBeCopied-bytesLeftToCopy;
				qint64 bytesJustDecompressed=this->bufferSize;
				this->bytesBuffered=bytesJustDecompressed;
				qint64 bytesToCopyNow=(bytesLeftToCopy<bytesJustDecompressed)?
				                      	bytesLeftToCopy
				                      :
				                      	bytesJustDecompressed;
				this->decompCur+=bytesToCopyNow;
				this->bufCur=bytesToCopyNow;
				memcpy(data+bytesAlreadyCopied,this->outputBuffer,bytesToCopyNow);
				bytesLeftToCopy-=bytesToCopyNow;
				if(0==bytesLeftToCopy){
					this->arcFile->seek(savedPos);
					return bytesToBeCopied;
				}
				else{
					memset(this->outputBuffer,0,this->bufferSize);
					this->z->next_out=this->outputBuffer;
				}
			}
			// the input data has ended
			else{
				qint64 bytesAlreadyCopied=bytesToBeCopied-bytesLeftToCopy;
				qint64 bytesDecompressedAll=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32)-bz2bytesReadErr;
				qint64 bytesJustDecompressed=bytesDecompressedAll-(bytesAlreadyCopied-bytesInBufferInitially);
				this->bytesBuffered=bytesJustDecompressed;
				qint64 bytesToCopyNow=(bytesLeftToCopy<bytesJustDecompressed)?
				                      	bytesLeftToCopy:
				                      	bytesJustDecompressed;
				this->decompCur+=bytesToCopyNow;
				this->bufCur=bytesToCopyNow;
				memcpy(&data[bytesAlreadyCopied],this->outputBuffer,bytesToCopyNow);
				bytesLeftToCopy-=bytesToCopyNow;
				if(0==bytesLeftToCopy){
					this->arcFile->seek(savedPos);
					return bytesToBeCopied;
				}
				else break;
			}
		}
	}
	return 0;
}

qint64 WAIFileReader::pos(){
	return this->decompCur;
}

/**
 * Starts reading and decompressing compressed file until the
 * given position (the first input parameter qint64 offset) is
 * reached in the decompressed data.
 *
 * After this function's completion:
 * this->arcCur    is set to the position of the first byte of
 *                 this->inputBuffer in the archive file;
 * this->compCur   is set to the same value, but relatively to the
 *                 compressed file beginning;
 * this->decompCur is set to the first input parameter qint64 offset
 *                 (the requested position to seek to in the terms of
 *                 decompressed data);
 * this->bufCur    is set to the value representing the byte of
 *                 this->outputBuffer which is requested to seek to;
 * the position of the cursor of the archive file is left untouched.
 */
bool WAIFileReader::seek(qint64 offset){
	// purging garbage
	memset(this->inputBuffer,0,this->bufferSize);
	memset(this->outputBuffer,0,this->bufferSize);

	// initializing bz2
	this->z=new bz_stream;
	this->z->bzalloc=0;
	this->z->bzfree=0;
	this->z->opaque=NULL;
	this->z->bzalloc=NULL;
	this->z->bzfree=NULL;
	this->z->next_in=this->inputBuffer;
	this->z->next_out=this->outputBuffer;
	this->z->avail_out=this->bufferSize;
	BZ2_bzDecompressInit(this->z,0,0);

	// seeking to zero
	qint64 savedPos=arcFile->pos();
	this->arcCur=fsNode->file()->property("arcPos").toLongLong();
	this->compCur=0;
	this->decompCur=0;
	this->bufCur=0;
	arcFile->seek(this->arcCur);

	qint64 outLen=0; // the amount of data we will get decompressed
	forever{
		// reading this->bufferSize bytes
		qint64 bytesRead=-1;
		if(!~(bytesRead=arcFile->read(this->inputBuffer,this->bufferSize)))
			throw E_WPIAI_FileReadError();
		this->arcCur+=bytesRead;
		this->compCur+=bytesRead;
		this->z->next_in=this->inputBuffer;

		// handling errors (reached the end of the archive)
		if(bytesRead<this->bufferSize)
			;

		// read more than needed: ran out of the compressed file data (overread compCur-compSize bytes)
		if(this->compCur>this->fsNode->file()->property("arcCompSize").toLongLong()){
			// calculating bytes of compressed data left available (til the end of it)
			this->z->avail_in=this->fsNode->file()->property("arcCompSize").toLongLong()-(this->compCur-bytesRead);

			// unrolling pos to the end of the comp file
			this->arcCur-=this->compCur-this->fsNode->file()->property("arcCompSize").toLongLong();
			this->compCur-=this->compCur-this->fsNode->file()->property("arcCompSize").toLongLong();
			arcFile->seek(this->arcCur);
		}
		// all fine, got this->bufferSize of compressed data
		else
			this->z->avail_in=this->bufferSize;

		// decompressing the whole this->inputBuffer
		forever{
			int rc=BZ2_bzDecompress(this->z);

			// an unrecoverable decompression error has occured
			if(BZ_PARAM_ERROR      == rc
			|| BZ_DATA_ERROR       == rc
			|| BZ_DATA_ERROR_MAGIC == rc
			|| BZ_MEM_ERROR        == rc
			) throw E_WPIAI_FileDecompressionError();

			// compressed file's end has been reached (all data has been decompressed)
			if(BZ_STREAM_END==rc){
				outLen=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);

				// if we've gotten our needed offset in this->outputBuffer, we're done
				if(offset<outLen){
					this->decompCur=offset;
					this->bufCur=offset%this->bufferSize;
					this->bytesBuffered=(0==outLen%this->bufferSize)?
					                    	this->bufferSize
							    :
					                    	outLen%this->bufferSize;
					this->arcFile->seek(savedPos);
					return true;
				}
				// otherwise, we're unable to seek
				else return this->arcFile->seek(savedPos),false;
			}

			// the output buffer has ended
			if(0==this->z->avail_out
			&& 0< this->z->avail_in){
				outLen=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);

				// if we've gotten our needed offset in this->outputBuffer, we're done
				if(offset<outLen){
					this->decompCur=offset;
					this->bufCur=offset%this->bufferSize;
					this->bytesBuffered=(0==outLen%this->bufferSize)?
					                    	this->bufferSize
							    :
					                    	outLen%this->bufferSize;
					this->arcFile->seek(savedPos);
					return true;
				}
				// otherwise purge the obtained decompressed data (we don't need it anyway)
				// to get the space for the new portion and continue
				else{
					memset(this->outputBuffer,0,this->bufferSize);
					this->z->next_out=this->outputBuffer;
				}
			}
			// the input data has ended
			else{
				outLen=(qint64(this->z->total_out_hi32)<<32)+qint64(this->z->total_out_lo32);

				// if we've gotten our needed offset in this->outputBuffer, we're done
				if(offset<outLen){
					this->decompCur=offset;
					this->bufCur=offset%this->bufferSize;
					this->bytesBuffered=(0==outLen%this->bufferSize)?
					                    	this->bufferSize
					                    :
					                    	outLen%this->bufferSize;
					this->arcFile->seek(savedPos);
					return true;
				}
				// otherwise break out for the new portion of input data
				else break;
			}
		}
	}
}

qint64 WAIFileReader::read(char *data,qint64 len,const WFile *file){
	WAIFileReader *reader=WAIFileReader::getReaderByFSNode(file->fsNode);
	return reader->read(data,len);
}

qint64 WAIFileReader::pos(const WFile *file){
	WAIFileReader *reader=WAIFileReader::getReaderByFSNode(file->fsNode);
	return reader->pos();
}

bool WAIFileReader::seek(qint64 offset,const WFile *file){
	WAIFileReader *reader=WAIFileReader::getReaderByFSNode(file->fsNode);
	return reader->seek(offset);
}

QList<WAIFileReader*> WAIFileReader::readersList;
WAIFileReader* WAIFileReader::getReaderByFSNode(WFileSystemNode *fsNode){
	bool readerAlreadyExists=false;
	WAIFileReader *reader;
	foreach(reader,WAIFileReader::readersList)
		if(reader->fsNode==fsNode){
			readerAlreadyExists=true;
			break;
		}
	if(readerAlreadyExists)
		return reader;
	else{
		reader=new WAIFileReader(fsNode);
		WAIFileReader::readersList<<reader;
		return reader;
	}
}

WAIFileReader::~WAIFileReader(){
	free(this->inputBuffer);
	free(this->outputBuffer);
}
