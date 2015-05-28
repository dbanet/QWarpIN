#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <QString>
#include <exception>
using std::exception;
#include "globals.h"
#define E_DESCR(x) public: virtual const char* what() const throw(){return x;}
#define E_EXC(x,y) class x : public exception{ E_DESCR(y) }

E_EXC(E_WAI_InstantiationError,
      "An archive interface is unable to instantiate itself.");
E_EXC(E_WAI_CannotReadArchiveFile,
      "An archive interface is unable to read the file.");
E_EXC(E_WA_CannotFindSuitableArchiveInterface,
      "WArchive is unable to find a suitable archive interface for the given archive file.");
E_EXC(E_WA_FileNotFound,
      "WArchive is unable to read the specified archive file (file not found).");
E_EXC(E_WFS_FileNotFound,
      "WFileSystem has failed to navigate to the specified file.");
E_EXC(E_WSH_InvalidContext,
      "WScriptHost has been requested to perform an operation in a context that does not support this.");
E_EXC(E_WPIAI_CannotReadArchiveHeader,
      "WarpINArchiveInterface is unable to find a correct header in the given archive file.");
E_EXC(E_WPIAI_FileReadError,
      "WarpINArchiveInterface has encountered a file read error.");
E_EXC(E_WPIAI_OutdatedArchive,
      "WarpINArchiveInterface does not support that old archive files.");
E_EXC(E_WPIAI_UnsupportedArchive,
      "A WarpIN archive has requested an unknown WarpIN version (probably a newer one), so WarpINArchiveInterface is unable to handle it.");
E_EXC(E_WPIAI_ErrorDecompressingInstallationScript,
      "WarpINArchiveInterface has failed to unbzip2 the installation script. It is probably corrupted.");
E_EXC(E_WPIAI_FileDecompressionError,
      "WarpINArchiveInterface has failed to unbzip2 one of the files in the archive. It is probably corrupted.");
E_EXC(E_WPIAI_ErrorReadingExtendedData,
      "WarpINArchiveInterface has failed to read the Extended Data information stored in the archive. It is probably corrupted.");
E_EXC(E_WPIAI_InvalidAmountOfPackagesInArchive,
      "A WarpIN archive claims to have invalid amount of packages in it. It is probably corrupted.");
E_EXC(E_WPIAI_InvalidAmountOfFilesInPackage,
      "A WarpIN archive claims to have invalid amount of files in one of its packages. It is probably corrupted.");
E_EXC(E_WPIAI_MaximumPathLengthExceededWhileReadingFiles,
      "A WarpIN archive contains a file with path length greater than the maximum length defined at compiletime, MAXPATH=" STRINGIZE(MAXPATH));
E_EXC(E_WPIAI_FileBelongsToUndefinedPackage,
      "A WarpIN archive contains a file claimed to belong to a package never defined in the archive. It is probably corrupted..");

#undef E_DESCR
#endif // EXCEPTIONS_H
