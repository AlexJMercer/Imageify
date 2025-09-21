#include "ErrorHandling.hpp"
#include <iostream>

void logError(const std::string& msg)
{
    std::cout << "\033[1;31m[ERROR] " << msg << "\033[0m" << std::endl;
}

std::string errorCodeToString(PNGManipErrorCode code)
{
    switch (code)
    {
        case PNGManipErrorCode::Success: return "Success";
    
        case PNGManipErrorCode::FileNotFound: return "File Not Found";
    
        case PNGManipErrorCode::FileNotReadable: return "File Not Readable";
    
        case PNGManipErrorCode::FileNotWritable: return "File Not Writable";
    
        case PNGManipErrorCode::InvalidFileFormat: return "Invalid File Format";
    
        case PNGManipErrorCode::EncodingError: return "Encoding Error";
    
        case PNGManipErrorCode::DecodingError: return "Decoding Error";
    
        case PNGManipErrorCode::MemoryAllocationError: return "Memory Allocation Error";
    
        case PNGManipErrorCode::UnknownError: return "Unknown Error";
    
        default: return "Unrecognized Error";
    }
}
