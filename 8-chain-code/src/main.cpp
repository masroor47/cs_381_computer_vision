#include <iostream>
#include <fstream>
#include <memory>
#include "ChainCode.h"

int main(int argc, const char* argv[]) {
    if (argc != 7) {
        std::cerr << "Your command line needs 6 parameters: 2 input files, 4 output files" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1]);
    std::ifstream prop_file(argv[2]);
    std::ofstream output_file(argv[3]);
    std::ofstream chain_code_file(argv[4]);
    std::ofstream boundary_file(argv[5]);
    std::ofstream log_file(argv[6]);

    if (!input_file.is_open() || 
        !prop_file.is_open() ||
        !output_file.is_open() || 
        !chain_code_file.is_open() ||
        !boundary_file.is_open() ||
        !log_file.is_open())
    {
        std::cerr << "Error opening files!" << std::endl;
        return 1;
    }

    int rows, cols, min_val, max_val;
    input_file >> rows >> cols >> min_val >> max_val;
    prop_file >> rows >> cols >> min_val >> max_val; // info duplicated
    int num_components;
    prop_file >> num_components;
    std::unique_ptr<CCproperty[]> CCproperties = std::make_unique<CCproperty[]>(num_components);
    for (int i = 0; i < num_components; i++) {
        prop_file >> CCproperties[i].label >> CCproperties[i].num_pixels >> CCproperties[i].min_row >> CCproperties[i].min_col >> CCproperties[i].max_row >> CCproperties[i].max_col;
    }

    ChainCode chain_code{rows, cols, min_val, max_val, num_components};
    chain_code.zero_framed();
    chain_code.load_image(input_file);

    output_file << "Below is the loaded image array of input " << std::endl;
    chain_code.pretty_dot_print(chain_code.image, output_file);

    chain_code_file << rows << " " << cols << " " << min_val << " " << max_val << std::endl;
    chain_code_file << num_components << std::endl;

    for (int i = 0; i < num_components; i++) {
        chain_code.CC.label = CCproperties[i].label;
        chain_code.CC.num_pixels = CCproperties[i].num_pixels;
        chain_code.CC.min_row = CCproperties[i].min_row;
        chain_code.CC.min_col = CCproperties[i].min_col;
        chain_code.CC.max_row = CCproperties[i].max_row;
        chain_code.CC.max_col = CCproperties[i].max_col;

        chain_code.clear_cc_ary();
        chain_code.load_cc_ary(chain_code.CC.label);

        log_file << "\nBelow is the loaded CC array of connected component " << chain_code.CC.label << std::endl;
        chain_code.pretty_dot_print(chain_code.CC_ary, log_file);

        chain_code_file << std::endl;
        chain_code.get_chain_code(chain_code_file, log_file);
    }

    chain_code_file.close();
    std::ifstream input_chain_code_file(argv[4]);

    // construct boundary
    chain_code.construct_boundary(input_chain_code_file);

    output_file << "*** Below is the objects boundaries of the input label image ***" << std::endl;
    chain_code.pretty_dot_print(chain_code.boundary, output_file);
    boundary_file << "*** Below is the objects boundaries of the input label image ***" << std::endl;
    chain_code.array_to_file(chain_code.boundary, boundary_file);
    
    output_file.close();
    chain_code_file.close();
    boundary_file.close();
    log_file.close();
}