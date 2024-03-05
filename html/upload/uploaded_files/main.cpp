#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main()
{
	std::vector<std::vector<std::string> > a(3);
	a[0].push_back("abc");
	a[1].push_back("def");
	a[2].push_back("ghi");
	std::cout << a[0][0] << std::endl;
	std::cout << a[1][0] << std::endl;
	std::cout << a[2][0] << std::endl;



	return 0;
}
