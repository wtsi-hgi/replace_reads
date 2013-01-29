#include <htslib/sam.h>
#include <cstring>
#include <string>

class ReplaceParser {

public:
    void parse_args(int argc, char** argv) {
        if (argc < 4) throw "owwie";
        
        file_name = argv[1];
        replace_name = argv[2];
        out_name = argv[3];
    }
    bool init() {
        // Open files
        file_iter = sam_open(file_name.c_str(), "rb", 0);
        replace_iter = sam_open(replace_name.c_str(), "rb", 0);
        out_file = sam_open(out_name.c_str(), "wb", 0);
        
        if (file_iter == NULL || replace_iter == NULL || out_file == NULL) {
            return false;
        }
        return true;
    }

    void parse() {
        // Iterate through BAM
        bam_hdr_t* file_header = sam_hdr_read(file_iter);
        bam_hdr_t* replace_header = sam_hdr_read(replace_iter);
        if (sam_hdr_write(out_file, file_header) != 0) {
            throw "IEEEE!";
        }
        
        bam1_t* file_read = bam_init1();
        bam1_t* replace_read = bam_init1();
        bam1_t* to_write = NULL;
        
        if (sam_read1(replace_iter, replace_header, replace_read) < 0 ) {
            throw "erk";
        }
        
        while (sam_read1(file_iter, file_header, file_read) >= 0) {
            // if read is not in replace file just write it
            if (replace_read != NULL && !strcmp(bam_get_qname(file_read), bam_get_qname(replace_read))
                && ((file_read->core.flag&(BAM_FREAD1|BAM_FREAD2)) == (replace_read->core.flag&(BAM_FREAD1|BAM_FREAD2)))) {
                // Match so write replacement
                sam_write1(out_file, file_header, replace_read);
                // Cue up next
                if ( sam_read1(replace_iter, file_header, replace_read) < 0 ) {
                    // No more reads to replace
                    bam_destroy1(replace_read);
                    replace_read = NULL;
                }
            } else {
                sam_write1(out_file, file_header, file_read);
            }
        }

        // Clean up
        if (replace_read) { bam_destroy1(replace_read); }
        if (file_read) { bam_destroy1(file_read); }
    }
    
    void cleanup() {
        sam_close(out_file);
        sam_close(replace_iter);
        sam_close(file_iter);
     }
    
private:
    std::string file_name;
    std::string replace_name;
    std::string out_name;

    samFile* file_iter;
    samFile* replace_iter;
    samFile* out_file;
};

int main(int argc, char** argv) {

    ReplaceParser parser;
    
	parser.parse_args(argc, argv);
    if (!parser.init()) return -1;
    
    parser.parse();
    
    parser.cleanup();
    
	return 0;
}
