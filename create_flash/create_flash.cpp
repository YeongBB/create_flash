#include<iostream>
#include<fstream>
#include<string>
#include<vector>
using namespace std;
int ch1 = 0, ch2 = 0, ch3 = 0;
int megaint = 0;					// 전역변수, 할당 메가 크기

vector<vector<vector<char>>> init()			// Flash 생성
{

	cout << " 할당할 메모리 크기를 입력하세요( mega 단위 1 ~ 4 ) : ";
	string megain;
	cin >> megain;
	while (megain.size() != 1 || atoi(megain.c_str()) <= 0 || atoi(megain.c_str()) > 4)
	{
		cout << " 다시 입력하세요 : ";
		cin >> megain;
	}
	megaint = atoi(megain.c_str());

	vector<vector<vector<char>>> mega;	// ex) 2048*32*16 = 1048576 -> 1mega
	vector<vector<char>> block_;		// 블록 크기 32+16=512byte
	vector<char> sector_;				// 섹터의 크기 16byte

	for (int b = 0; b < 32; b++)			// block	32칸
	{
		for (int c = 0; c < 16; c++)		// sector is 16bit
		{
			sector_.push_back(' ');
		}
		block_.push_back(sector_);
	}

	for (int a = 0; a < megaint * 2048; a++)			// Flash mega input*2048
		mega.push_back(block_);
	cout << " " << megaint << "Mb생성(" << megaint * 2048 * 32 * 16 << "Byte)" << endl;
	return mega;
}
vector<vector<vector<char>>> Flash_read(vector<vector<vector<char>>> memo)	// read
{
	ifstream fp;								// 파일변수
	char line;
	string filename;
	cout << " 파일 이름 입력 : ";
	cin >> filename;
	int a = 0, b = 0, c = 0;
	fp.open(filename);
	if (fp.is_open())
	{
		while (!fp.eof())							// 파일 읽기
		{
			fp.get(line);
			if (line != '\n')
			{
				if (c > 15)						//16이상이면 다음 섹터에 쓰기
				{
					c = 0;
					b++;
				}
				if (b > 31)						// 블록 개수를 넘으면 다음 블록에 쓰기
				{
					b = 0;
					a++;
				}
				if (a > memo.size() && b == 31 && c == 15)	// 정한 메모리 최대용량 넘을때 종료		
					break;
				memo[a][b][c] = line;
				c++;
			}
		}
	}
	else
	{
		cout << " 일치하는 파일이 없습니다. 프로그램을 종료합니다." << endl;
		exit(0);
	}
	fp.close();
	return memo;
}
vector<vector<vector<char>>> Flash_write(vector<vector<vector<char>>> memo)		//  write
{
	int write_int = 0;
	cout << " 쓸 위치를 입력하세요 : ";
	string write_s;
	cin >> write_s;
	while (atoi(write_s.c_str()) < 0 || atoi(write_s.c_str()) > memo.size() * 32)
	{
		cout << " 다시 입력하세요 : ";
		cin >> write_s;
	}
	write_int = atoi(write_s.c_str());
	int write_init = write_int / 32;
	int write_in = write_int % 32;
	string write_str;
	cout << " 쓸 데이터를 입력하세요. :";
	cin >> write_str;
	int check_copy = 0;
	int  r = 0;
	int cop = 0;
	if (memo[write_init][write_in][0] != ' ')			// 데이터를 쓸 곳에 이미 데이터가 있다면
	{
		int check = 0;
		for (int b = 0; b < 32; b++)
		{
			for (int c = 0; c < 16; c++)
			{
				if (memo[memo.size()][b][c] == ' ')
				{
					memo[memo.size()][b][c] = memo[write_init][b][c];	// 다른곳으로 복제
					memo[memo.size()][b][c] = ' ';						// 원레 쓸 곳을 삭제
				}
				check++;
				if (check == 512)							// 블록 전부다 할당시 종료
					break;
			}
			check_copy++;
		}
	}
	for (int a = 0; a < write_str.size(); a++)					// 쓰기
	{
		if (r > 16)
		{
			write_in = +1;
			if (write_in > 31)
			{
				write_init += 1;
				write_in = 0;
			}
			r = 0;
		}
		memo[write_init][write_in][r] = write_str[a];
		r++;
	}

	ch1 = write_init;
	ch2 = write_in;
	if (check_copy > 0)									// 쓰고 난 뒤 데이터 붙여놓기
	{
		for (int a = 0; a < 32; a++)
		{
			for (int b = 0; b < 16; b++)
			{
				if (memo[write_init][a][b] == ' ')			// 그 자리에 데이터가 없을때만 
					memo[write_init][a][b] = memo[cop][a][b];
			}
		}
	}

	return memo;
}
vector<vector<vector<char>>> Flash_erase(vector<vector<vector<char>>> memo)	// erase
{
	for (int a = 0; a < memo.size(); a++)		// ' ' 으로 초기화 
	{
		for (int b = 0; b < 32; b++)
		{
			for (int c = 0; c < 16; c++)
			{
				memo[a][b][c] = ' ';
			}
		}
	}
	return memo;
}

int main()
{
	system("mode con cols=80 lines=50");
	vector<vector<vector<char>>> Flash_;
	Flash_ = init();
	Flash_ = Flash_read(Flash_);		// PSN read
	Flash_ = Flash_write(Flash_);		// PSN write
	cout << " write 위치  : " << ch1 << " ," << ch2 << endl;	// 임시, 지정 위치에 데이터가 잘 써졌는지 확인
	for (int a = 0; a < 16; a++)
	{
		cout << Flash_[ch1][ch2][a] << endl;
	}											// ~~//	
	Flash_ = Flash_erase(Flash_);		// erase	
	return 0;
}

