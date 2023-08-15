#include <iostream>

enum class MyFileLibraryError {
    FileNotFound = 1000,
    FileAlreadyExists = 2000,
    FileBusy = 3000,
    FileTooBig = 4000
};

class MyFileLibraryCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "MyFileLibrary";
    }

    std::string message(int ev) const override {
        switch (static_cast<MyFileLibraryError>(ev)) {
        case MyFileLibraryError::FileNotFound:
            return "The file was not found";
        case MyFileLibraryError::FileBusy:
            return "The file is busy";
        case MyFileLibraryError::FileAlreadyExists:
            return "The file already exists";
        case MyFileLibraryError::FileTooBig:
            return "The file too big and can't be processed";
        default:
            return "Unsupported error";
        }
    }

    bool equivalent(int code, const std::error_condition& condition) const noexcept override {
        switch (static_cast<MyFileLibraryError>(code)) {
        case MyFileLibraryError::FileAlreadyExists:
            return condition == std::errc::file_exists;
        case MyFileLibraryError::FileNotFound:
            return condition == std::errc::no_such_file_or_directory;
        case MyFileLibraryError::FileBusy:
            return condition == std::errc::text_file_busy;
        case MyFileLibraryError::FileTooBig:
            return condition == std::errc::file_too_large;
        default:
            return false;
        }
    }
    
};

const MyFileLibraryCategory my_file_lib_category{};

int main() {
    std::error_code file_exists{static_cast<int>(MyFileLibraryError::FileAlreadyExists), 
                                my_file_lib_category};
    if (file_exists == std::errc::file_exists) {
        std::cout << "Msg: " << file_exists.message() << '\n';
        std::cout << "Category: " << file_exists.default_error_condition().category().name() << '\n';
    }

    return 0;
}