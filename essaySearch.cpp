
#define FILE_EXTENSION ".txt"
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <queue>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <unordered_map>
using namespace std;
namespace fs = std::filesystem;
struct Trie
{
	char c;
	int table[26] = {};
	Trie *next[26];
	unordered_map<int, int> lastid;
	unordered_map<int, int> titlesid;
	Trie() {}
};
Trie *prefix_head;
Trie *suffix_head;
Trie *prefix_cur;
Trie *suffix_cur;
vector<string> essayid;
int essay_num;
// Utility Func
// string parser : output vector of strings (words) after parsing
vector<string> word_parse(vector<string> tmp_string)
{
	vector<string> parse_string;
	for (auto &word : tmp_string)
	{
		string new_str;
		for (auto &ch : word)
		{
			if (isalpha(ch))
			{
				new_str.push_back(ch >= 'a' ? ch : ch + 32);
				// new_str.push_back(ch);
			}
		}
		parse_string.emplace_back(new_str);
	}
	return parse_string;
}

vector<string> split(const string &str, const string &delim)
{
	vector<string> res;
	if ("" == str)
		return res;
	// 先將要切割的字串從string型別轉換為char*型別
	char *strs = new char[str.length() + 1]; // 不要忘了
	strcpy(strs, str.c_str());

	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p)
	{
		string s = p;	  // 分割得到的字串轉換為string型別
		res.push_back(s); // 存入結果陣列
		p = strtok(NULL, d);
	}

	return res;
}

int main(int argc, char *argv[])
{
	clock_t start = clock();
	string data_dir = argv[1] + string("/");
	string query = string(argv[2]);
	string output = string(argv[3]);

	string file, title_name, tmp;
	fstream fi;
	vector<string> tmp_string;

	// init
	prefix_head = new Trie();
	prefix_head->c = '#';
	suffix_head = new Trie();
	suffix_head->c = '#';
	int count = 0;
	for (const auto &path : fs::directory_iterator(data_dir))
		count++;
	for (int k = 0; k < count; k++)
	{
		string current_path = data_dir + to_string(k) + ".txt";
		fi.open(current_path, ios::in);
		getline(fi, title_name);
		tmp_string = split(title_name, " ");
		essayid.emplace_back(title_name);
		// cout << current_path << '\n';
		vector<string> title = word_parse(tmp_string);
		// build two trie
		for (auto &word : title)
		{
			prefix_cur = prefix_head;
			suffix_cur = suffix_head;
			for (char &c : word)
			{
				if (prefix_cur->table[c - 'a'] == 0)
				{
					prefix_cur->table[c - 'a'] = 1;
					Trie *create = new Trie();
					create->c = c;
					prefix_cur->next[c - 'a'] = create;
					prefix_cur = create;
				}
				else
				{
					prefix_cur = prefix_cur->next[c - 'a'];
				}
				prefix_cur->titlesid[essay_num] = 1;
			}
			prefix_cur->lastid[essay_num] = 1;
			for (int i = word.size() - 1; i >= 0; i--)
			{
				char c = word[i];
				if (suffix_cur->table[c - 'a'] == 0)
				{
					suffix_cur->table[c - 'a'] = 1;
					Trie *create = new Trie();
					create->c = c;
					suffix_cur->next[c - 'a'] = create;
					suffix_cur = create;
				}
				else
				{
					suffix_cur = suffix_cur->next[c - 'a'];
				}
				suffix_cur->titlesid[essay_num] = 1;
			}
		}

		while (getline(fi, tmp))
		{

			tmp_string = split(tmp, " ");

			vector<string> content = word_parse(tmp_string);

			for (auto &word : content)
			{
				// cout << word << endl;
				prefix_cur = prefix_head;
				suffix_cur = suffix_head;
				for (char &c : word)
				{
					if (prefix_cur->table[c - 'a'] == 0)
					{
						prefix_cur->table[c - 'a'] = 1;
						Trie *create = new Trie();
						create->c = c;
						prefix_cur->next[c - 'a'] = create;
						prefix_cur = create;
					}
					else
					{
						prefix_cur = prefix_cur->next[c - 'a'];
					}
					prefix_cur->titlesid[essay_num] = 1;
				}
				prefix_cur->lastid[essay_num] = 1;
				for (int i = word.size() - 1; i >= 0; i--)
				{
					char c = word[i];
					if (suffix_cur->table[c - 'a'] == 0)
					{
						suffix_cur->table[c - 'a'] = 1;
						Trie *create = new Trie();
						create->c = c;
						suffix_cur->next[c - 'a'] = create;
						suffix_cur = create;
					}
					else
					{
						suffix_cur = suffix_cur->next[c - 'a'];
					}
					suffix_cur->titlesid[essay_num] = 1;
				}
			}
		}
		essay_num++;
		fi.close();
	}

	fstream input_file;
	std::ofstream ofs;
	string op;
	Trie *current;
	vector<string> op_splitted;
	bool found;
	input_file.open(query, ios::in);
	ofs.open(output);

	while (getline(input_file, op))
	{
		char last_op = '#';
		vector<int> result(essay_num);
		op_splitted = split(op, " ");
		for (string &s : op_splitted)
		{
			found = true;
			if (s[0] == '*')
			{
				reverse(s.begin(), s.end());
				current = suffix_head;
				for (int i = 1; i < s.size() - 1; i++)
				{
					char c = s[i];
					if (current->table[c - 'a'] == 0)
					{
						found = false;
						break;
					}
					else
						current = current->next[c - 'a'];
				}
			}
			else if (s[0] == '"')
			{
				current = prefix_head;
				for (int i = 1; i < s.size() - 1; i++)
				{
					char c = s[i];
					if (current->table[c - 'a'] == 0)
					{
						found = false;
						break;
					}
					else
						current = current->next[c - 'a'];
				}
				// if (current->last)
				// found = false;
			}
			else if (s == "/")
			{
				last_op = '/';
				continue;
			}
			else if (s == "+")
			{
				last_op = '+';
				continue;
			}
			else
			{
				current = prefix_head;
				for (char &c : s)
				{
					if (current->table[c - 'a'] == 0)
					{
						found = false;
						break;
					}
					else
						current = current->next[c - 'a'];
				}
			}

			if (found)
			{
				if (last_op == '/' || last_op == '#')
				{
					if (s[0] == '"')
					{
						for (int i = 0; i < essay_num; i++)
						{
							if (current->lastid[i] == 1)
							{
								result[i] = 1;
							}
						}
					}
					else
					{
						for (int i = 0; i < essay_num; i++)
						{
							if (current->titlesid[i] == 1)
							{
								result[i] = 1;
							}
						}
					}
				}
				else if (last_op == '+')
				{
					if (s[0] == '"')
					{
						for (int i = 0; i < essay_num; i++)
						{
							if (current->lastid[i] == 0)
							{
								result[i] = 0;
							}
						}
					}
					else
					{
						for (int i = 0; i < essay_num; i++)
						{
							if (current->titlesid[i] == 0)
							{
								result[i] = 0;
							}
						}
					}
				}
			}
			// not found and last operand is '+' -> all zero
			else if (last_op == '+')
			{
				for (int i = 0; i < essay_num; i++)
				{
					result[i] = 0;
				}
			}
		}
		bool notfound = true;
		for (int i = 0; i < essay_num; i++)
		{
			if (result[i] == 1)
			{
				ofs << essayid[i] << '\n';
				notfound = false;
			}
		}
		if (notfound)
			ofs << "Not Found!\n";
	}
	input_file.close();
	ofs.close();
	clock_t stop = clock();
	// cout << (double)(stop - start) / CLOCKS_PER_SEC;
}
