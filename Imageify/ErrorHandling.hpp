#ifndef ERROR_HANDLING_H_
#define ERROR_HANDLING_H_

#include <string>
#include <system_error>


enum class PNGManipErrorCode
{
	Success = 0,
	FileNotFound,
	FileNotReadable,
	FileNotWritable,
	InvalidFileFormat,
	EncodingError,
	DecodingError,
	MemoryAllocationError,
	UnknownError
};

// Logs error message in bright red to terminal
void logError(const std::string& msg);

// Converts error code to string
std::string errorCodeToString(PNGManipErrorCode code);


#endif // !ERROR_HANDLING_H_
