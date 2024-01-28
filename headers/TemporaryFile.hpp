#ifndef TEMPORARYFILE_HPP
# define TEMPORARYFILE_HPP
# include "common.hpp"
# include "utils.hpp"

class TemporaryFile {
public:
    TemporaryFile(const std::string& content="", const std::string& type="");
	TemporaryFile(const TemporaryFile &other);
    ~TemporaryFile();
    void setContent(const std::string& content);
	void setFilename(const std::string& name);
	std::string readContent() const; 
	std::string filename() const;
private:
    std::string filename_;
	std::string type_;

    void generateUniqueFilename();
};

#endif