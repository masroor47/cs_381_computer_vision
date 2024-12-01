#include <memory>

struct point {
    int row;
    int col;
};
struct CCproperty {
    int label;
    int num_pixels;
    int min_row, min_col, max_row, max_col;
};

class ChainCode {
    public:
        int num_cc;
        CCproperty CC;
        int num_rows;
        int num_cols;
        int min_val;
        int max_val;

        std::unique_ptr<int[]> image;
        std::unique_ptr<int[]> boundary;
        std::unique_ptr<int[]> CC_ary;

        std::unique_ptr<point[]> coord_offset;

        int zero_table[8] = {6, 0, 0, 2, 2, 4, 4, 6};

        point start_p;
        point current_p;
        point next_p;
        int last_q;
        int next_dir;
        int p_chain_dir;

        ChainCode(int, int, int, int, int);
        void zero_framed();
        void load_image(std::ifstream&);
        void clear_cc_ary();
        void load_cc_ary(int);
        void get_chain_code(std::ofstream&, std::ofstream&);
        int find_next_p(point, int, std::ofstream&);
        void construct_boundary(std::ifstream&);
        void pretty_dot_print(const std::unique_ptr<int[]>&, std::ofstream&);
        void pretty_print();
        void array_to_file(std::unique_ptr<int[]>&, std::ofstream&);
};