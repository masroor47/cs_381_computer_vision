#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "ChainCode.h"

ChainCode::ChainCode(int rows, 
                     int cols, 
                     int min_val,
                     int max_val,
                     int numCCs) :
                                    num_cc(numCCs),
                                    num_rows(rows), 
                                    num_cols(cols), 
                                    min_val(min_val), 
                                    max_val(max_val)
{
    image = std::make_unique<int[]>((num_rows+2) * (num_cols+2));
    boundary = std::make_unique<int[]>((num_rows+2) * (num_cols+2));
    CC_ary = std::make_unique<int[]>((num_rows+2) * (num_cols+2));
    coord_offset = std::make_unique<point[]>(8);
    coord_offset[0] = { 0, 1 };
    coord_offset[1] = { -1, 1 };
    coord_offset[2] = { -1, 0 };
    coord_offset[3] = { -1, -1 };
    coord_offset[4] = { 0, -1 };
    coord_offset[5] = { 1, -1 };
    coord_offset[6] = { 1, 0 };
    coord_offset[7] = { 1, 1 };
};

void ChainCode::zero_framed() {
    for (int i = 0; i < num_cols + 2; i++) {
        image[i] = 0;
        image[(num_rows + 1) * (num_cols + 2) + i] = 0;
    }
    for (int i = 0; i < num_rows + 2; i++) {
        image[i * (num_cols + 2)] = 0;
        image[i * (num_cols + 2) + num_cols + 1] = 0;
    }
};

void ChainCode::load_image(std::ifstream& in) {
    for (int i = 1; i < num_rows + 1; i++) {
        for (int j = 1; j < num_cols + 1; j++) {
            in >> image[i * (num_cols + 2) + j];
        }
    }
};

void ChainCode::clear_cc_ary() {
    for (int i = 0; i < num_rows + 2; i++) {
        for (int j = 0; j < num_cols + 2; j++) {
            CC_ary[i * (num_cols + 2) + j] = 0;
        }
    }
}

void ChainCode::load_cc_ary(int label) {
    // from image to CC_ary, only copy the pixels with the same label
    for (int i = 1; i < num_rows + 1; i++) {
        for (int j = 1; j < num_cols + 1; j++) {
            if (image[i * (num_cols + 2) + j] == label) {
                CC_ary[i * (num_cols + 2) + j] = label;
            }
        }
    }
}

void ChainCode::get_chain_code(std::ofstream& chain_code_file, std::ofstream& log_file) {
    log_file << "Entering get_chain_code() method" << std::endl;
    chain_code_file << num_rows << " " << num_cols << " " << min_val << " " << max_val << std::endl;
    
    int label = CC.label;

    bool found = false;
    for (int r = 1; r < num_rows + 1; r++) {
        for (int c = 1; c < num_cols + 1; c++) {
            if (CC_ary[r * (num_cols + 2) + c] == label) {
                chain_code_file << r << " " << c << " " << label << std::endl;  // Starting point
                start_p = { r, c };
                current_p = { r, c };
                last_q = 4;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    bool first_move = true;
    
    while (true) {
        next_dir = (last_q + 1) % 8;
        p_chain_dir = find_next_p(current_p, next_dir, log_file);
        
        // Output just the direction and a space
        chain_code_file << p_chain_dir << " ";

        log_file << "currentP = (" << current_p.row << ", " << current_p.col << ") ";

        // Update current position directly
        current_p.row += coord_offset[p_chain_dir].row;
        current_p.col += coord_offset[p_chain_dir].col;

        // Update last_q based on p_chain_dir
        if (p_chain_dir == 0) {
            last_q = zero_table[7];
        } else {
            last_q = zero_table[p_chain_dir - 1];
        }

        log_file << "lastQ = " << last_q << "; nextQ = " << next_dir 
                << ", nextP = (" << current_p.row + coord_offset[p_chain_dir].row
                << ", " << current_p.col + coord_offset[p_chain_dir].col
                << ")"
                << std::endl;

        // Only break if we've moved at least once and returned to start
        if (!first_move && current_p.row == start_p.row && current_p.col == start_p.col) {
            break;
        }
        first_move = false;
    }

    chain_code_file << std::endl;  // End the chain code with newline
    log_file << "Exiting get_chain_code() method" << std::endl;
}

int ChainCode::find_next_p(point current_p, int last_q, std::ofstream& log_file) {
    log_file << "Entering find_next_p() method" << std::endl;
    
    int index = last_q;
    int chain_dir = -1;
    bool found = false;
    int start_index = index;
    
    while (!found) {
        int row = current_p.row + coord_offset[index].row;
        int col = current_p.col + coord_offset[index].col;
        int pixel_value = CC_ary[row * (num_cols+2) + col];
        
        log_file << "In find_next_p(), checking index: " << index 
                << " at position (" << row << ", " << col << ")"
                << ", pixel value: " << pixel_value 
                << std::endl;
        
        if (pixel_value == CC.label) {
            chain_dir = index;
            found = true;
            log_file << "Found valid pixel at direction " << chain_dir << std::endl;
        } else {
            index = (index + 1) % 8;
            if (index == start_index) {
                log_file << "Error: Checked all directions, no valid neighbor found" << std::endl;
                return -1;  // Error condition
            }
        }
    }
    
    log_file << "Exiting find_next_p() method, returning chain_dir: " << chain_dir << std::endl;
    return chain_dir;
}

void ChainCode::pretty_print() {
    for (int i = 0; i < num_rows+2; i++) {
        for (int j = 0; j < num_cols+2; j++) {
            std::cout << image[i * (num_cols+2) + j] << " ";
        }
        std::cout << std::endl;
    }
}

void ChainCode::pretty_dot_print(const std::unique_ptr<int[]>& ary, std::ofstream& out) {
    for (int i = 0; i < num_rows+2; i++) {
        for (int j = 0; j < num_cols+2; j++) {
            out << ary[i * (num_cols+2) + j] << " ";
        }
        out << std::endl;
    }
}

void ChainCode::construct_boundary(std::ifstream& chain_code_file) {
    int rows, cols, min_val, max_val, num_components, start_row, start_col, label;
    chain_code_file >> rows >> cols >> min_val >> max_val;
    chain_code_file >> num_components;

    for (int i = 0; i < num_components; i++) {
        chain_code_file >> rows >> cols >> min_val >> max_val;
        chain_code_file >> start_row >> start_col >> label;
        std::string line;
        chain_code_file.ignore();
        std::getline(chain_code_file, line);
        std::stringstream ss(line);
        std::vector<int> directions;
        int dir;
        while (ss >> dir) {
            directions.push_back(dir);
        }

        for (int j = 0; j < directions.size(); j++) {
            boundary[start_row * (num_cols+2) + start_col] = label;
            start_row += coord_offset[directions[j]].row;
            start_col += coord_offset[directions[j]].col;
        }
    }

}

void ChainCode::array_to_file(std::unique_ptr<int[]>& ary, std::ofstream& out) {
    for (int i = 0; i < num_rows+2; i++) {
        for (int j = 0; j < num_cols+2; j++) {
            out << ary[i * (num_cols+2) + j] << " ";
        }
        out << std::endl;
    }
}