#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <QString>
#include <exception>
using std::exception;
#define E_DESCR(x) public: virtual const char* what() const throw(){return x;}
#define E_EXC(x,y) class x : public exception{ E_DESCR(y) }

E_EXC(E_WAI_InstantiationError,
      "An archive interface is unable to instantiate itself.");
E_EXC(E_WAI_CannotReadArchiveFile,
      "An archive interface is unable to read the file.");
E_EXC(E_WA_CannotFindSuitableArchiveInterface,
      "WArchive is unable to find a suitable archive interface for the given archive file.");
E_EXC(E_WPIAI_CannotReadArchiveHeader,
      "WarpinArchiveInterface is unable to find a correct header in the given archive file.");
E_EXC(E_WPIAI_OutdatedArchive,
      "WarpinArchiveInterface does not support that old archive files.");
E_EXC(E_WPIAI_UnsupportedArchive,
      "A WarpIN archive has requested an unknown WarpIN version (probably a newer one), so WarpinArchiveInterface is unable to handle it.");
E_EXC(E_WPIAI_ErrorDecompressingInstallationScript,
      "WarpinArchiveInterface has failed to unbzip2 the installation script. It is probably corrupted.");
E_EXC(E_WPIAI_ErrorReadingExtendedData,
      "WarpinArchiveInterface has failed to read the Extended Data information stored in the archive. It is probably corrupted.");
E_EXC(E_WPIAI_InvalidAmountOfPackagesInArchive,
      "A WarpIN archive claims to have invalid amount of packages in it. It is probably corrupted.")

#undef E_DESCR
#endif // EXCEPTIONS_H
