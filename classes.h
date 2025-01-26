 /*** This is just a Skeleton/Starter Code for the External Storage Assignment. This is by no means absolute, in terms of assignment approach/ used functions, etc. ***/
/*** You may modify any part of the code, as long as you stick to the assignments requirements we do not have any issue ***/

// Include necessary standard library headers
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std; // Include the standard namespace

class Record {
public:
    int id, manager_id; // Employee ID and their manager's ID
    std::string bio, name; // Fixed length string to store employee name and biography

    Record() {}

    Record(vector<std::string> &fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    //You may use this for debugging / showing the record to standard output. 
    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }

    int get_size(){
        return int(sizeof(int) * 2 + bio.length() + name.length());
    }
     // Serialize the record for writing to file
    string serialize() const {
        ostringstream oss;
        oss.write(reinterpret_cast<const char*>(&id), sizeof(id));
        oss.write(reinterpret_cast<const char*>(&manager_id), sizeof(manager_id));
        int name_len = name.size();
        int bio_len = bio.size();
        oss.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        oss.write(name.c_str(), name.size());
        oss.write(reinterpret_cast<const char*>(&bio_len), sizeof(bio_len));
        oss.write(bio.c_str(), bio.size());
        return oss.str();
    }

    void deserialize(const string& binary_data) {
        istringstream iss(binary_data);

        // Read `id`
        iss.read(reinterpret_cast<char*>(&id), sizeof(id));

        // Read `manager_id`
        iss.read(reinterpret_cast<char*>(&manager_id), sizeof(manager_id));

        // Read length of `name` and then `name` itself
        int name_len = 0;
        iss.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));

        name.resize(name_len); // Allocate space for the name
        iss.read(&name[0], name_len);

        // Read length of `bio` and then `bio` itself
        int bio_len = 0;
        iss.read(reinterpret_cast<char*>(&bio_len), sizeof(bio_len));

        bio.resize(bio_len); // Allocate space for the bio
        iss.read(&bio[0], bio_len);

        // Validation (optional)
        if (iss.fail()) {
            throw runtime_error("Failed to deserialize the record");
        }
    }

};

class page{ // Take a look at Figure 9.7 and read Section 9.6.2 [Page Organization for Variable Length Records] 
public:
    vector <Record> records; // This is the Data_Area, which contains the records. 
    vector <pair <int, int> > slot_directory; // This slot directory contains the starting position (offset), and size of the record.
                                             // The starting position i refers to the location of record i in the records and size of that particular record i. 
                                             // This information is crucial, you can load record using this if you know if your record is starting from, e.g., 
                                            // byte 128 and has size 70, you read all characters from [128 to 128 + 70]

    
    int cur_size = 0; // holds the current size of the
    int free_space = 4096; //FMA. hold where the free space is, plan to use it to determine the offset
    // Write a Record into your page
    bool insert_record_into_page(Record r){
        int record_size = r.serialize().size();
        // Take a look at Figure 9.9 and read the Section 9.7.2 [Record Organization for Variable Length Records]
        // You may adopt any of the approaches mentioned their. E.g., id $ name $ bio $ manager_id $ separating records with a delimiter / the alternative approaches
        if(cur_size + record_size >= 4096){     // Check the current size of your page. Your page has 4KB memory for storing the records and the slot directory information.
            //You cannot insert the current record into this page
            return false;
        }
        else {
            records.push_back(r);
            // update slot directory information
            slot_directory.push_back(make_pair(cur_size, record_size));
            cur_size += record_size;
            return true;
        }
    }

    // void write_into_data_file(ostream& out) const {
    //     //Write the records and slot directory information into your data file. You are basically writing 4KB into the datafile.
    //     //You must maintain a fixed size of 4KB so there may be some unused empty spaces.
    //
    //     char page_data[4096] = {0}; // Let's write all the information of the page into this char array. So that we can write the page into the data file in one go.
    //     int offset = 0; // position of free space
    //
    //     // If you look at figure 9.7, you'll find that there are spaces allocated for the slot-directory.
    //     // You can structure your page in your own way, such as allocate first x bytes of memory to store the slot-directory information
    //     //  sizeof(int) bytes to parse these numbers.
    //     // After those x bytes, you start storing your records.
    //     // You can definitely use $ (delimiter) while storing the slot directory informations /(or,) as you know that these are integers(sizeof(int)) you can read
    //
    //     for (const auto& record : records) {
    //         string serialized = record.serialize();
    //
    //         memcpy(page_data + offset, serialized.c_str(), serialized.size());
    //
    //         offset += serialized.size();
    //     }
    //     //the above loop just read the id, name, bio etc. You'll also need to store the slot-directory information. So that you can use the slot-directory
    //     // to retrieve a record.
    //     for (const auto& slots : slot_directory) {
    //         // insert the slot directory information into the page_data
    //     }
    //
    //     // Write the page_data to the EmployeeRelation.dat file
    //     out.write(page_data, sizeof(page_data)); // Always write exactly 4KB
    // }

    // Function to write the page to a binary output stream, i.e., EmployeeRelation.dat file
    void write_into_data_file(ostream& out) const {
        //Write the records and slot directory information into your data file. You are basically writing 4KB into the datafile.
        //You must maintain a fixed size of 4KB so there may be some unused empty spaces.

        char page_data[4096] = {0}; // Let's write all the information of the page into this char array. So that we can write the page into the data file in one go.
        int offset = 0;

        /*FMA*/

        int dir_offset = 4096; // Adding directory at the bottom
        int num_slots =0;       // The number of slots that we are using for the current page
        //FMA


        // If you look at figure 9.7, you'll find that there are spaces allocated for the slot-directory.
        // You can structure your page in your own way, such as allocate first x bytes of memory to store the slot-directory information
        //  sizeof(int) bytes to parse these numbers.
        // After those x bytes, you start storing your records.
        // You can definitely use $ (delimiter) while storing the slot directory informations /(or,) as you know that these are integers(sizeof(int)) you can read

        for (const auto& record : records) {
            string serialized = record.serialize();

            memcpy(page_data + offset, serialized.c_str(), serialized.size());

            offset += serialized.size();
        }
        //the above loop just read the id, name, bio etc. You'll also need to store the slot-directory information. So that you can use the slot-directory
        // to retrieve a record.
        // Insert Number of slot 4096 - sizeof(int)
        //Intro number of slots
        num_slots += slot_directory.size();
        dir_offset -= sizeof(int);
        memcpy(page_data + dir_offset, &num_slots, sizeof(int));

        for (const auto& slots : slot_directory) {
        /*FMA*/
            // insert the slot directory information into the page_data
            int record_offset = slots.first;
            int record_size = slots.second;

            //Intro duces record offset
            dir_offset -= sizeof(int); //MO ves the pointer of the directory
            memcpy(page_data + dir_offset, &record_offset, sizeof(int));

            //Introduces recod length
            dir_offset -= sizeof(int);
            memcpy(page_data + dir_offset, &record_size, sizeof(int));
        }
        /*FMA*/

        // Write the page_data to the EmployeeRelation.dat file
        out.write(page_data, sizeof(page_data)); // Always write exactly 4KB
    }

    // Read a page from a binary input stream, i.e., EmployeeRelation.dat file to populate a page object
    bool read_from_data_file(istream& in) {
        // Read all the records and slot_directory information from your .dat file
        // Remember you are reading a chunk of 4098 byte / 4 KB from the data file to your main memory.
        char page_data[4096] = {0};
        in.read(page_data, 4096);

        streamsize bytes_read = in.gcount();
        // You may populate the records and slot_directory from the 4 KB data you just read.
        if (bytes_read == 4096) {

            // Process data to fill the slot directory and the records to handle it according to the structure
            // Assuming slot directory is processed here or elsewhere depending on your serialization method
            // Reconstruct the slot directory
            // Start rebuilding the slot_directory from the bottom of the page
            int dir_offset = 4096;
            slot_directory.clear();
            records.clear();

            // Read the number of slots
            dir_offset -= sizeof(int);
            int num_slots;
            memcpy(&num_slots, page_data + dir_offset, sizeof(int));

            // Rebuild the slot directory
            for (int i = 0; i < num_slots; ++i) {
                dir_offset -= sizeof(int);
                int record_offset;
                memcpy(&record_offset, page_data + dir_offset, sizeof(int));

                dir_offset -= sizeof(int);
                int record_size;
                memcpy(&record_size, page_data + dir_offset, sizeof(int));

                slot_directory.push_back(make_pair(record_offset, record_size));
            }

            for (const auto& slots : slot_directory) {
                // Reconstruct the records
                int record_offset = slots.first;
                int record_size = slots.second;
                // Extract the serialized record data
                string record_string(page_data + record_offset, record_size);
                Record r;
                r.deserialize(record_string);
                records.push_back(r);
            }

            return true;
        }

        if (bytes_read > 0) {
            cerr << "Incomplete read: Expected " << 4096 << " bytes, but only read " << bytes_read << " bytes." << endl;
        }

        // Reset the stream state for subsequent reads
        if (in.eof() || in.fail()) {
            in.clear();            // Clear EOF or error flags
            in.seekg(0, ios::end); // Move to the end of the file (optional reset position)
        }

        return false;
    }

    void clear() {
        records.clear();
        slot_directory.clear();
        cur_size = 0;
        free_space = 4096;
    }

    bool is_empty() const {
        return slot_directory.empty(); // A page is empty if it has no records
    }
};

class StorageManager {

public:
    string filename;  // Name of the file (EmployeeRelation.dat) where we will store the Pages 
    fstream data_file; // fstream to handle both input and output binary file operations
    vector <page> buffer; // You can have maximum of 3 Pages.
    
    // Constructor that opens a data file for binary input/output; truncates any existing data file
    StorageManager(const string& filename) : filename(filename) {
        data_file.open(filename, ios::binary | ios::out | ios::in | ios::trunc);
        if (!data_file.is_open()) {  // Check if the data_file was successfully opened
            cerr << "Failed to open data_file: " << filename << endl;
            exit(EXIT_FAILURE);  // Exit if the data_file cannot be opened
        }
    }

    // Destructor closes the data file if it is still open
    ~StorageManager() {
        if (data_file.is_open()) {
            data_file.close();
        }
    }

    // Reads data from a CSV file and writes it to a binary data file as Employee objects
    void createFromFile(const string& csvFilename) {
        buffer.resize(3); // You can have maximum of 3 Pages.

        ifstream csvFile(csvFilename);  // Open the Employee.csv file for reading
        
        string line;
        int page_number = 0; // Current page we are working on [at most 3 pages]

        // Read each line from the CSV file, parse it, and create Employee objects
        while (getline(csvFile, line)) {
            stringstream ss(line);
            string item;
            vector<string> fields;
            // Split line by commas
            while (getline(ss, item, ',')) {
                fields.push_back(item);
            }
            //create A record object            
            Record r = Record(fields);

            if (!buffer[page_number].insert_record_into_page(r)) {

                // If the current page is full, move to the next page
                page_number++;
 
                // If you have used all your 3 pages and they are filled up with records you may use write_into_data_file() to write the pages into the data file
                // It is like dumping all the information in you pages (Which are in your main memory) into the .dat file (which is in External Storage)
                if (page_number >= buffer.size()) {
                    for (page& p : buffer) {
                        p.write_into_data_file(data_file);
                        // Reset/Free the pages and start filling them up with the future records
                        p.clear();
                    }
                    page_number = 0;
                }
                // Insert the record into the new current page
                buffer[page_number].insert_record_into_page(r);
            }
            // Write any remaining pages in the buffer to the file after the loop
            for (int i = 0; i < buffer.size(); ++i) {
                if (!buffer[i].is_empty()) { // Check if the page is not empty
                    buffer[i].write_into_data_file(data_file);
                    buffer[i].clear(); // Reset the page
                }
            }
            
        }
        csvFile.close();  // Close the CSV file
    }

    /**
     * Searches for an Employee by ID in the binary data_file using the page and prints if found
     * @param searchId
     */
    void findAndPrintEmployee(int searchId) {
        data_file.seekg(0, ios::beg);  // Rewind the data_file to the beginning for reading
        int page_number = 0;
        int page_offset = 0;  // Keeps track of the number of pages read so far
        const int page_size = 4096; // Size of one page (4KB)
        bool found = false;
        while (buffer[page_number].read_from_data_file(data_file)) {
            // Now process the current page using the slot directory to find the desired id
            // Process logic goes here
            for (Record record : buffer[page_number].records) {
                if (record.id == searchId) {
                    record.print();
                    found = true;
                    break;
                }
            }
            if (found) break;
            page_number++;
            page_offset++;
            // If buffer is full, reset `page_number` to 0 for circular usage
            if (page_number >= buffer.size()) {
                page_number = 0;
                // Ensure the file stream has advanced correctly
                // Skip over the already-read pages (3 pages, 4KB each in this case)
                data_file.seekg(page_offset * page_size, ios::beg);
                // If the seek goes beyond EOF, it will stop further reading
                if (data_file.eof()) {
                    break;
                }
            }
        }
        // Print not found message if no match from any of the records
        if (!found) {
            cout << "Employee with ID " << searchId << " not found." << endl;
        }
    }
};
