#include <iostream>
#include <iterator>
#include <string>
#include <regex>
#include <map>
#include <fstream>

int main()
{
    std::string service = "^(tel|fax|(sms))\\:";
    std::string number = "\\+[0-9]{11}";
    std::string text = "[0-9a-zA-Z\\.\\,\\?\\!]{1,64}";
    std::string fname;
    std::cout << "Write name of file: ";
    std::cin >> fname;
    std::ifstream fdata(fname);
    if (!fdata.is_open())
    {
    	std::cout << "Failed to open the file." << std::endl;
    	return 1;
    }
    std::map<std::string, int> nums;
    if (fdata.good())
    {
        std::string line;
        while (std::getline(fdata, line))
        {
            std::regex num_regex(number);
            auto words_begin = std::sregex_iterator(line.begin(), line.end(), num_regex);
            auto words_end = std::sregex_iterator();
            int k = std::distance(words_begin, words_end);
            if (k == 0) {
                std::cout << "Not correct." << std::endl;
                continue;
            }
            std::string res_reg = "^(tel|fax|(sms))\\:(\\+[0-9]{11}\\,){k-1}\\+[0-9]{11};body=(?(2)[0-9a-zA-Z\\.\\,\\?\\!]{1,64}|)$";
            std::regex serv(res_reg); // the pattern
            bool match_res = std::regex_search(line, serv);
            if (match_res) {
                std::cout << "Correct." << std::endl;
                for (std::sregex_iterator i = words_begin; i != words_end; ++i)
                {
                    std::smatch match = *i;
                    std::string match_str = match.str();
                    if (nums.count(match_str))
                        nums[match_str]++;
                    else    
                        nums[match_str] = 1;
                }
            }
        }
    }
    fdata.close();
    std::ofstream res;
    res.open("regex_res.txt");
    for (auto it = nums.begin(); it != nums.end(); ++it)
        std::cout << it->first << ": " << it->second << '\n';
    res.close();
    return 0;
}
