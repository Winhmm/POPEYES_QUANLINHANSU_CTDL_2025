#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <conio.h>
#include <fstream>
#include <iomanip>
#include <limits>
#include <direct.h> 
#include <windows.h> 
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

/**********************************************************
* @Description Các hằng số cấu hình hệ thống
**********************************************************/
const string FILE_ADMIN = "Administrators.txt";
const string FILE_EMPLOYEES = "Employees.txt";
const string DIR_INFO = "Employees_Information";
const string DEFAULT_PASS = "111111";

/**********************************************************
* @Description Bảng màu
**********************************************************/
enum Color {
	BLACK = 0, BLUE = 1, GREEN = 2, CYAN = 3, RED = 4, MAGENTA = 5, BROWN = 6,
    LIGHTGRAY = 7, DARKGRAY = 8, LIGHTBLUE = 9, LIGHTGREEN = 10, LIGHTCYAN = 11,
    LIGHTRED = 12, LIGHTMAGENTA = 13, YELLOW = 14, WHITE = 15
};

/**********************************************************
* @Description Thiết lập màu sắc cho văn bản console
*
* @param color Mã màu (enum Color)
* @return Không có
* @attention Sử dụng Windows API (SetConsoleTextAttribute)
**********************************************************/
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

/**********************************************************
* @Description Kiểm tra và tạo thư mục lưu trữ thông tin
*
* @return Không có
* @attention Sử dụng _mkdir để tạo thư mục nếu chưa tồn tại
* @attention Folder mặc định là "Employees_Information"
**********************************************************/
void ensureDirectoryExists() {
    struct stat info;
    if(stat(DIR_INFO.c_str(), &info) != 0) {
        _mkdir(DIR_INFO.c_str());
    }
}

/**********************************************************
* @Description Chuẩn hóa chuỗi tên (Viết hoa chữ cái đầu mỗi từ)
*
* @param strName Chuỗi tên cần chuẩn hóa
* @return Chuỗi tên đã được chuẩn hóa
* @attention Ví dụ: "nguyen van a" -> "Nguyen Van A"
**********************************************************/
string standardizeNames(string strName) {
	stringstream ss(strName);
	string strWord, strRes = "";
	while(ss>>strWord) {
		strWord[0] = toupper(strWord[0]);
		for(size_t i = 1; i < strWord.size(); i++) {
			strWord[i] = tolower(strWord[i]);
		}
		strRes += strWord + " ";
	}
	if(!strRes.empty()) {
		strRes.pop_back();
	}
	return strRes;
}

/**********************************************************
* @Description Xóa khoảng trắng thừa ở đầu và cuối chuỗi
*
* @param strInput Chuỗi cần xử lý
* @return Chuỗi đã loại bỏ khoảng trắng thừa
* @attention Trả về chuỗi rỗng nếu đầu vào chỉ toàn khoảng trắng
**********************************************************/
string trim(const string& strInput) {
	size_t iFirst = strInput.find_first_not_of(" \t\r\n");
    if (string::npos == iFirst) return "";
    size_t iLast = strInput.find_last_not_of(" \t\r\n");
    return strInput.substr(iFirst, (iLast - iFirst + 1));
}

/**********************************************************
* @Description Kiểm tra tính hợp lệ của số điện thoại
*
* @param strPhone Chuỗi số điện thoại
* @return true nếu hợp lệ, false nếu không hợp lệ
* @attention SĐT phải có độ dài đúng bằng 10, bắt đầu bằng 0 và chỉ chứa số
**********************************************************/
bool isValidPhoneNumber(string strPhone) {
	if(strPhone.length() != 10) {
		return false;
	}

	if(strPhone[0] != '0') {
		return false;
	}

	for(char c : strPhone) {
		if(!isdigit(c)) {
			return false;
		}
	}
	return true;
}

/**********************************************************
* @Description Kiểm tra tính hợp lệ của địa chỉ Gmail
*
* @param strEmail Chuỗi email
* @return true nếu hợp lệ, false nếu không hợp lệ
* @attention Phải kết thúc bằng "@gmail.com"
* @attention Cho phép chữ, số, dấu chấm và dấu gạch dưới
**********************************************************/
bool isValidGmail(string strEmail) {
	string strSuffix = "@gmail.com";
	if(strEmail.length() <= strSuffix.length()) {
		return false;
	}
	string strTail = strEmail.substr(strEmail.length() - strSuffix.length());
	if(strTail != strSuffix) {
		return false;
	}

	string strUsername = strEmail.substr(0, strEmail.length() - strSuffix.length());
	if(strUsername.find('@') != string::npos) {
		return false;
	}
	if(strUsername.find("gmail") != string::npos) {
		return false;
	}
	if(strUsername.find(".com") != string::npos) {
		return false;
	}
	for(char c : strUsername) {
		if(!isalnum(c) && c != '.'&& c != '_') {
			return false;
		}
	}
	return true;
}

/**********************************************************
* @Description Kiểm tra định dạng Username
*
* @param strUser Chuỗi username
* @return true nếu hợp lệ, false nếu lỗi
* @attention Độ dài tối thiểu 3, không chứa ký tự đặc biệt (trừ _)
**********************************************************/
bool isValidUsernameFormat(string strUser) {
	if(strUser.length() < 3) {
		return false;
	}
	for(char c : strUser) {
		if(!isalnum(c) && c != '_') {
			return false;
		}
	}
	return true;
}

/**********************************************************
* @Description Kiểm tra định dạng Họ tên
*
* @param strName Chuỗi họ tên
* @return true nếu hợp lệ
* @attention Không chứa số và ký tự đặc biệt
**********************************************************/
bool isValidName(string strName) {
	if(strName.length() < 2) {
		return false;
	} 
	for(char c : strName) {
		if(!isalpha(c) && !isspace(c)) {
			return false;
		}
	}
	return true;
}

/**********************************************************
* @Description Nhập mật khẩu hiển thị dưới dạng dấu '*'
*
* @return Chuỗi mật khẩu người dùng đã nhập
* @attention Xử lý phím Backspace để xóa ký tự
* @attention Kết thúc khi nhấn Enter (mã ASCII 13)
**********************************************************/
string getHiddenPassword() {
	string strPassword = "";
	char cHidden;
	while(true) {
		cHidden = _getch();
		if(cHidden == 13) { // Enter
			break;
		} else if(cHidden == 8) { // Backspace
			if(!strPassword.empty()) {
				cout<<"\b \b";
				strPassword.pop_back();
			}
		} else {
			strPassword.push_back(cHidden);
			cout<<"*";
		}
	}
	cout<<endl;
	return strPassword;
}


/**********************************************************
* @Description Template hàm nhập liệu an toàn
*
* @param strPrompt Thông báo nhắc người dùng nhập
* @return Giá trị kiểu T người dùng nhập vào
* @attention Tự động xóa bộ đệm và yêu cầu nhập lại nếu sai kiểu dữ liệu
**********************************************************/
template<typename T>
T getInput(string strPrompt) {
	T tValue;
	while(true) {
		setColor(LIGHTCYAN);
		cout<<strPrompt;
		setColor(WHITE);
		if(cin>>tValue) {
			string strRemain;
			getline(cin, strRemain);
			return tValue;
		} else {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			setColor(LIGHTRED);
			cout<<"Du lieu khong hop le, vui long nhap lai"<<endl;
		}
	}
}

// --- Template nhập node ---
/**********************************************************
* @Description Class Node đại diện cho một phần tử trong DSLK
**********************************************************/
template<typename T>
class Node{
	private:
		T _data;
		Node<T>* _pNext;
	public:
		Node() : _data(T()), _pNext(nullptr) {}
		Node(T val) : _data(val), _pNext(nullptr) {}
		
		/**********************************************************
        * @Description Gán dữ liệu cho Node
        * @param Value Giá trị cần gán
        **********************************************************/
		void SetData(T Value){
			_data = Value; 
		}

		/**********************************************************
        * @Description Thiết lập con trỏ Next
        * @param pNext Con trỏ đến Node tiếp theo
        **********************************************************/
		void SetNext(Node* pNext){ 
			_pNext = pNext; 
		}

		/**********************************************************
        * @Description Lấy dữ liệu của Node
        * @return Tham chiếu đến dữ liệu
        **********************************************************/
		T& GetData() { 
			return _data;
		}

		/**********************************************************
        * @Description Lấy con trỏ đến Node tiếp theo
        * @return Pointer Node kế tiếp
        **********************************************************/
		Node* GetNext() { 
			return _pNext; 
		}	
};


// --- Class danh sách liên kết đơn ---
/**********************************************************
* @Description Class LinkedList quản lý danh sách liên kết
**********************************************************/
template <class T>
class LinkedList {
private:

	Node<T>* _pHead;
    Node<T>* _pTail;

public:
	LinkedList() : _pHead(nullptr), _pTail(nullptr) {}

	/**********************************************************
    * @Description Destructor: Giải phóng bộ nhớ danh sách
    *
    * @return Không có
    * @attention Duyệt và delete từng node trong danh sách
    **********************************************************/
	~LinkedList() {
		Node<T>* pCurrent = _pHead;
		while(pCurrent != nullptr) {
			Node<T>* pNext = pCurrent->GetNext();
			delete pCurrent;
			pCurrent = pNext;
		}
		_pHead = nullptr;
		_pTail = nullptr;
	}

	/**********************************************************
    * @Description Thêm phần tử vào cuối danh sách
    *
    * @param value Giá trị cần thêm
    * @return Không có
    * @attention Cập nhật cả pHead và pTail
    **********************************************************/
	void pushBack(T value) {
        Node<T>* pNewNode = new Node<T>(value);
        if (!_pHead) { 
            _pHead = pNewNode;
            _pTail = pNewNode;
        } else {
            _pTail->SetNext(pNewNode); 
            _pTail = pNewNode;
        }
    }

	/**********************************************************
    * @Description Duyệt danh sách và thực hiện hành động
    *
    * @param action Hàm lambda/function thực hiện trên mỗi phần tử
    * @return Không có
    **********************************************************/
	void forEach(function<void(T)> action) {
		Node<T>* p = _pHead;
		while(p) {
			action(p->GetData()); 
            p = p->GetNext();
		}
	}

	/**********************************************************
    * @Description Tìm kiếm phần tử thỏa mãn điều kiện
    *
    * @param predicate Hàm điều kiện tìm kiếm
    * @return Giá trị T tìm thấy
    * @attention Trả về nullptr (hoặc giá trị mặc định) nếu không thấy
    **********************************************************/
	T find(function<bool(T)> predicate) {
		Node<T>* p = _pHead;
		while(p) {
			if (predicate(p->GetData())) { 
                return p->GetData();
            }
			p = p->GetNext();
		}
		return nullptr;
	}

	/**********************************************************
    * @Description Kiểm tra sự tồn tại của phần tử
    *
    * @param predicate Hàm điều kiện kiểm tra
    * @return true nếu tồn tại, false nếu không
    **********************************************************/
	bool exists(function<bool(T)> predicate) {
		Node<T>* p = _pHead;
		while(p) {
			if (predicate(p->GetData())) { 
                return true;
            }
			p = p->GetNext();
		}
		return false;
	}

	/**********************************************************
    * @Description Xóa phần tử thỏa mãn điều kiện
    *
    * @param predicate Hàm điều kiện để xác định phần tử xóa
    * @return true nếu xóa thành công, false nếu không tìm thấy
    * @attention Xử lý cả trường hợp xóa đầu, giữa và cuối
    **********************************************************/
	bool removeIf(function<bool(T)> predicate) {
		if(!_pHead) {
			return false;
		}

		if(predicate(_pHead->GetData())) {
			Node<T>* pTemp = _pHead;
			_pHead = _pHead->GetNext();

			if(_pHead == nullptr) {
				_pTail = nullptr;
			}
			delete pTemp;
			return true;
		}

		Node<T>* pCur = _pHead;
		Node<T>* pPrev = nullptr;
		while(pCur) {
			if(predicate(pCur->GetData())) {
				pPrev->SetNext(pCur->GetNext());

				if(pCur == _pTail) {
					_pTail = pPrev;
				}
				delete pCur;
				return true;
			}
			pPrev = pCur;
			pCur = pCur->GetNext();
		}
		return false;
	}

	/**********************************************************
    * @Description Kiểm tra danh sách rỗng
    * @return true nếu rỗng
    **********************************************************/
	bool isEmpty() const {
		return _pHead == nullptr;
	}

	/**********************************************************
    * @Description Sắp xếp nổi bọt (Bubble Sort) trên danh sách
    *
    * @param compare Hàm so sánh hai phần tử
    * @return Không có
    * @attention Đổi chỗ trực tiếp dữ liệu (Swap data) giữa các node
    **********************************************************/
	void bubbleSort(function<bool(T, T)> compare) {
		if(!_pHead || !_pHead->GetNext()) {
			return;
		}
		bool swapped;
		Node<T>* ptr1;
		Node<T>* lptr = nullptr;

		do {
			swapped = false;
			ptr1 = _pHead;

			while(ptr1->GetNext() != lptr) {
				if(compare(ptr1->GetData(), ptr1->GetNext()->GetData())) {
					swap(ptr1->GetData(), ptr1->GetNext()->GetData());
                    swapped = true;
				}
				ptr1 = ptr1->GetNext();
			}
			lptr = ptr1;
		} while(swapped);
	}
};


// --- Class Stack (dùng cho Undo) ---
/**********************************************************
* @Description Class Stack (LIFO) sử dụng Node
**********************************************************/
template <class T>
class Stack {
private:
    Node<T>* _pTop;
public:
    Stack() : _pTop(nullptr) {}

    ~Stack() {
        while (!isEmpty()) pop();
    }

	/**********************************************************
    * @Description Đẩy phần tử vào đỉnh Stack (Push)
    *
    * @param val Giá trị cần thêm
    * @return Không có
    **********************************************************/
    void push(const T& val) {
        Node<T>* newNode = new Node<T>(val);
        newNode->SetNext(_pTop);
        _pTop = newNode;
    }

	/**********************************************************
    * @Description Lấy và xóa phần tử ở đỉnh Stack (Pop)
    *
    * @return Giá trị phần tử ở đỉnh
    * @attention Trả về giá trị mặc định nếu Stack rỗng
    **********************************************************/
    T pop() {
        if (isEmpty()) {
            return T();
        }
        Node<T>* temp = _pTop;
        T val = temp->GetData();
        _pTop = _pTop->GetNext();
        delete temp;
        return val;
    }

	/**********************************************************
    * @Description Xem phần tử ở đỉnh Stack (Peek)
    * @return Giá trị phần tử đỉnh
    **********************************************************/
    T top() const {
        if (isEmpty()) return T();
        return _pTop->GetData();
    }

	/**********************************************************
    * @Description Kiểm tra Stack rỗng
    * @return true nếu rỗng
    **********************************************************/
    bool isEmpty() const {
        return _pTop == nullptr;
    }
};

/**********************************************************
* @Description Class cơ sở User
**********************************************************/
class User {
protected:
	string _strUsername;
    string _strPassword;
public:
	User(string strUser = "", string strPass = "") : _strUsername(strUser), _strPassword(strPass) {}
    virtual ~User() {}

    string getUsername() const { 
		return _strUsername; 
	}
    string getPassword() const { 
		return _strPassword; 
	}
    void setUsername(string strUser) {
		_strUsername = strUser; 
	}
    void setPassword(string strPass) { 
		_strPassword = strPass; 
	}
};

/**********************************************************
* @Description Class Employee kế thừa từ User
**********************************************************/
class Employee : public User {
private:
	string _strName;
    string _strAddress;
    string _strPhonenumber;
    string _strEmail;
public:
	Employee(string strUser = "", string strPass = "", string strName = "", string strAddr = "", string strPhone = "", string strEmail = "")
    	: User(strUser, strPass), _strName(strName), _strAddress(strAddr), _strPhonenumber(strPhone), _strEmail(strEmail) {}

	Employee(const Employee &other) : User(other._strUsername, other._strPassword) {
        _strName = other._strName;
        _strAddress = other._strAddress;
        _strPhonenumber = other._strPhonenumber;
        _strEmail = other._strEmail;
    }

	string getName() const { 
		return _strName; 
	}
    string getAddress() const { 
		return _strAddress; 
	}
    string getPhonenumber() const { 
		return _strPhonenumber; 
	}
    string getEmail() const { 
		return _strEmail; 
	}

    void setName(string strName) { 
		_strName = strName; 
	}
    void setAddress(string strAddr) { 
		_strAddress = strAddr; 
	}
    void setPhonenumber(string strPhone) { 
		_strPhonenumber = strPhone; 
	}
    void setEmail(string strEmail) { 
		_strEmail = strEmail; 
	}

	/**********************************************************
    * @Description Nạp chồng toán tử << để in thông tin Employee
    *
    * @param os Luồng đầu ra (ostream)
    * @param e Đối tượng Employee cần in
    * @return Reference đến ostream
    **********************************************************/
	friend ostream& operator<<(ostream& os, const Employee& e) {
		os<<left<<setw(15)<<e._strUsername<<setw(25) << e._strName<<setw(15)<<e._strPhonenumber<<setw(25)<<e._strEmail<<setw(20)<<e._strAddress;
        return os;
	}
};

/**********************************************************
* @Description Class quản lý danh sách nhân viên
**********************************************************/
class EmployeeManager {
private:
	LinkedList<Employee*> _listEmployees;
	Stack<Employee*> _undoStack;

	/**********************************************************
    * @Description Ghi thông tin chi tiết ra file [username].txt
    *
    * @param pEmp Pointer trỏ đến đối tượng Employee
    * @return Không có
    * @attention File được lưu trong thư mục DIR_INFO
    **********************************************************/
	void writeDetailFile(Employee* pEmp) {
		ensureDirectoryExists();
		string strFilename = DIR_INFO + "/" + pEmp->getUsername() + ".txt";
		ofstream fout(strFilename);
		if(fout.is_open()) {
			fout<<pEmp->getName()<<endl;
			fout<<pEmp->getAddress()<<endl;
			fout<<pEmp->getPhonenumber()<<endl;
			fout<<pEmp->getEmail()<<endl;
			fout.close();
		}
	}

	/**********************************************************
    * @Description Xóa file thông tin chi tiết
    *
    * @param strUsername Tên đăng nhập của nhân viên
    * @return Không có
    * @attention Sử dụng hàm remove của hệ thống
    **********************************************************/
	void deleteDetailFile(string strUsername) {
		string strFilename = DIR_INFO + "/" + strUsername + ".txt";
		if(remove(strFilename.c_str()) == 0) {
			setColor(GREEN);
			cout<<"Da xoa file thong tin chi tiet: "<<strFilename<<endl;
			setColor(WHITE);
		}
	}

	/**********************************************************
    * @Description Đọc thông tin chi tiết từ file txt
    *
    * @param pEmp Pointer Employee cần cập nhật thông tin
    * @return Không có
    * @attention File đọc dựa trên Username của pEmp
    **********************************************************/
	void readDetailFile(Employee* pEmp) {
		string strFilename = DIR_INFO + "/" + pEmp->getUsername() + ".txt";
		ifstream fin(strFilename);
		if(fin.is_open()) {
			string line;
			getline(fin, line);
        	pEmp->setName(trim(line));

        	getline(fin, line);
        	pEmp->setAddress(trim(line));

        	getline(fin, line);
        	pEmp->setPhonenumber(trim(line));
			
        	getline(fin, line);
        	pEmp->setEmail(trim(line));
		}
	}

public:
	EmployeeManager() {
        loadFromFile();
    }

	~EmployeeManager() {
    	_listEmployees.forEach([](Employee* e) {
        	delete e;
        });

		while(!_undoStack.isEmpty()) {
            delete _undoStack.pop();
        }
    }

	/**********************************************************
    * @Description Thêm nhân viên mới vào hệ thống
    *
    * @param pEmp Pointer trỏ đến Employee mới
    * @return Không có
    * @attention Thêm vào list, lưu file tổng và tạo file chi tiết
    **********************************************************/
	void addEmployee(Employee* pEmp) {
		_listEmployees.pushBack(pEmp);
		saveToFile();
		writeDetailFile(pEmp);
	}

	/**********************************************************
    * @Description Kiểm tra Username đã tồn tại chưa
    *
    * @param strUsername Username cần kiểm tra
    * @return true nếu đã tồn tại, false nếu chưa
    **********************************************************/
	bool isUsernameExist(string strUsername) {
		return _listEmployees.exists([&](Employee* e) {
        	return e->getUsername() == strUsername;
    	});
	}

	/**********************************************************
    * @Description Lấy đối tượng Employee theo Username
    *
    * @param strUsername Username cần tìm
    * @return Pointer Employee tìm thấy, hoặc nullptr nếu không có
    **********************************************************/
	Employee* getEmployeeByUsername(string strUsername) {
		return _listEmployees.find([&](Employee* e) {
        	return e->getUsername() == strUsername;
    	});
	}


	/**********************************************************
    * @Description Xóa nhân viên khỏi hệ thống
    *
    * @param strUsername Username của nhân viên cần xóa
    * @return Không có
    * @attention Lưu bản sao vào Stack trước khi xóa để hỗ trợ Undo
    * @attention Kiểm tra tồn tại trước khi xử lý để tránh lỗi Crash
    **********************************************************/
	void deleteEmployee(string strUsername) {
		Employee* target = getEmployeeByUsername(strUsername);

		if(target == nullptr) {
			setColor(RED);
			cout<<"Khong tim thay nhan vien voi username: "<<strUsername<<endl;
			setColor(WHITE);
			return;
		}

		Employee* copyEmp = new Employee(*target);
        _undoStack.push(copyEmp);
		
		bool bDeleted = _listEmployees.removeIf([&](Employee* e) {
            if (e->getUsername() == strUsername) {
                delete e;
                return true;
            }
            return false;
        });

		if(bDeleted) {
			saveToFile();
			deleteDetailFile(strUsername);
			setColor(GREEN);
			cout<<"Da xoa nhan vien thanh cong"<<endl;
			setColor(WHITE);
		} else {
			setColor(RED);
			cout<<"Khong tim thay username nay"<<endl;
			setColor(WHITE);
		}
	}

	/**********************************************************
    * @Description Khôi phục nhân viên vừa xóa (Undo)
    *
    * @return Không có
    * @attention Lấy từ Stack và đẩy lại vào List và File
    **********************************************************/
	void  undoDelete() {
		if(_undoStack.isEmpty()) {
			setColor(YELLOW);
			cout<<"Khong co gi de Undo"<<endl;
			setColor(WHITE);
			return;
		}

		Employee* restoredEmp = _undoStack.pop();
		_listEmployees.pushBack(restoredEmp);
		saveToFile();
		writeDetailFile(restoredEmp);
		cout<<"Da khoi phuc nhan vien: "<<restoredEmp->getUsername()<<endl;
		setColor(GREEN);
	}

	/**********************************************************
    * @Description Cập nhật thông tin nhân viên
    *
    * @return Không có
    * @attention Nhập dữ liệu mới, validation và lưu lại file
    **********************************************************/
	void updateEmployeeInfo() {
		string strUsername;
		cout<<"Nhap username can cap nhat: ";
		getline(cin, strUsername);
		strUsername = trim(strUsername);

		Employee* pEmp = getEmployeeByUsername(trim(strUsername));
		if(!pEmp) {
			setColor(RED);
			cout<<"Khong tim thay nhan vien voi username: "<<strUsername<<endl;
			setColor(WHITE);
			return;
		}

		setColor(YELLOW);
		cout<<"--- Thong tin hien tai ---"<<endl;
		cout << *pEmp << endl;
		setColor(WHITE);

		string strInput;
		cout<<"--- Cap nhat thong tin (an Enter neu giu nguyen) ---"<<endl;

		cout<<"Ho ten ("<<pEmp->getName()<<"): "; 
		getline(cin, strInput);
		if(!strInput.empty()) {
			if(isValidName(strInput)) {
				pEmp->setName(standardizeNames(strInput));
			} else {
				setColor(RED);
				cout<<"Ten khong duoc chua so/ky tu dac biet -> Giu nguyen ten cu"<<endl;
				setColor(WHITE);
			}
		}

		cout<<"Dia chi ("<<pEmp->getAddress()<<"): "; 
		getline(cin, strInput);
		if(!strInput.empty()) {
			pEmp->setAddress(strInput);
		}

		while(true) {
			cout<<"SDT (" << pEmp->getPhonenumber()<<"): ";
			getline(cin, strInput);

			if(strInput.empty()) {
				break;
			}

			if(isValidPhoneNumber(trim(strInput))) {
				pEmp->setPhonenumber(trim(strInput));
				break;
			} else {
				setColor(RED);
				cout<<"So dien thoai phai co dung 10 chu so: "<<endl;
				setColor(WHITE);
			}
		}

		while(true) {
			cout<<"Email (" << pEmp->getEmail() << "): ";
			getline(cin, strInput);

			if (strInput.empty()) {
                break; 
            }

			if(isValidGmail(trim(strInput))) {
				pEmp->setEmail(trim(strInput));
				break;
			} else {
				setColor(RED);
				cout<<"Email phải co duoi @gmail.com"<<endl;
				setColor(WHITE);
			}
		}

		saveToFile();
        writeDetailFile(pEmp);
		setColor(GREEN);
		cout<<"Cap nhat thanh cong"<<endl;
		setColor(WHITE);
	}

	/**********************************************************
    * @Description Sắp xếp danh sách nhân viên theo tên
    *
    * @return Không có
    * @attention Sử dụng thuật toán Bubble Sort
    **********************************************************/
	void sortEmployees() {
		if(_listEmployees.isEmpty()) {
			cout<<"Danh sach rong"<<endl;
			return;
		}
		_listEmployees.bubbleSort([](Employee* a, Employee* b) {
            return a->getName() > b->getName();
        });
	}

	/**********************************************************
    * @Description Hiển thị danh sách tất cả nhân viên
    *
    * @return Không có
    * @attention In ra dạng bảng sau khi đã sắp xếp
    **********************************************************/
	void showAll() {
		if(_listEmployees.isEmpty()) {
			setColor(YELLOW);
			cout<<"Danh sach rong"<<endl;
			setColor(WHITE);
			return;
		}
		sortEmployees();
		setColor(CYAN);
		cout<<left<<setw(15)<<"USERNAME"<<setw(25)<<"HO TEN"<<setw(15)<<"SDT"<<setw(25)<<"EMAIL"<<setw(20)<<"DIA CHI"<<endl;
		cout<<string(100, '-')<<endl;
		setColor(WHITE);

		_listEmployees.forEach([](Employee* e) {
            cout<<*e<<endl;
        });
	}

	/**********************************************************
    * @Description Load dữ liệu nhân viên từ file hệ thống
    *
    * @return Không có
    * @attention Đọc file Employees.txt và các file chi tiết tương ứng
    **********************************************************/
	void loadFromFile() {
		ifstream fin(FILE_EMPLOYEES);
		if(!fin.is_open()) {
			return;
		}
		string strLine;
		while(getline(fin, strLine)) {
			if(strLine.empty()) {
				continue;
			}
			
			// Loại bỏ BOM
			if(strLine.size() >= 3 && (unsigned char)strLine[0] == 0xEF) {
				strLine = strLine.substr(3);
			}

			stringstream ss(strLine);
			string strSegment;
			vector<string> vecFields;

			while(getline(ss, strSegment, ',')) {
				vecFields.push_back(trim(strSegment));
			}

			if(vecFields.size() >= 2) {
				Employee* pEmp = new Employee(vecFields[0], vecFields[1]);
				readDetailFile(pEmp);
                _listEmployees.pushBack(pEmp);
			}
		}
		fin.close();
	}

	void saveToFile() {
		ofstream fout(FILE_EMPLOYEES);
		_listEmployees.forEach([&](Employee* e) {
			fout<<e->getUsername()<< ", "<<e->getPassword()<<endl;
		});
		fout.close();
	}

	/**********************************************************
    * @Description Xác thực đăng nhập nhân viên
    *
    * @param strUser Tên đăng nhập
    * @param strPass Mật khẩu
    * @return Pointer Employee nếu đúng, nullptr nếu sai
    **********************************************************/
	Employee* login(string strUser, string strPass) {
		return _listEmployees.find([&](Employee* e) {
            return e->getUsername() == strUser && e->getPassword() == strPass;
        });
	}

	/**********************************************************
    * @Description Hiển thị thông tin cá nhân của người dùng
    *
    * @param strUsername Tên đăng nhập cần xem
    * @return Không có
    * @attention Đọc trực tiếp từ file chi tiết của user đó
    **********************************************************/
	void showMyInfoFromFile(string strUsername) {
		string strFilename = DIR_INFO + "/" + strUsername + ".txt";
		ifstream fin(strFilename);
		if(!fin.is_open()) {
			setColor(RED);
			cout<<"Khong tim thay file thong tin chi tiet ("<<strFilename<<")"<<endl;
			setColor(WHITE);
			return;
		}
		string strLine;
		setColor(CYAN);
		cout<<"--- THONG TIN CUA BAN (tu file "<<strFilename<<") ---"<<endl;
		setColor(WHITE);
		vector<string> vecLabels = {"Ho ten: ", "Dia chi: ", "SDT: ", "Email: "};
		int iIndex = 0;
		while(getline(fin, strLine) && iIndex < 4) {
			setColor(YELLOW);
			cout<<left<<setw(10)<<vecLabels[iIndex]<<strLine<<endl;
			setColor(WHITE);
			iIndex++;
		}
		fin.close();
	}
};

/**********************************************************
* @Description Class Application điều khiển luồng chương trình
**********************************************************/
class Application {
private:
	EmployeeManager _manager;
    Employee* _pCurrentUser;

	/**********************************************************
    * @Description Xử lý đăng nhập Admin (Có bảo mật)
    *
    * @return true nếu đăng nhập thành công, false nếu thất bại
    * @attention Đọc tài khoản từ file FILE_ADMIN
    * @attention Ràng buộc: Tối đa 3 lần thử, File không được rỗng
    **********************************************************/
	bool adminLogin() {
		ifstream fin(FILE_ADMIN);
		if(!fin.is_open()) {
			setColor(RED); 
			cout<<"Khong tim thay file"<<FILE_ADMIN<<endl;
			setColor(WHITE);
			return false;
		}

		string strFileUser, strFilePass;
		if(!(fin >> strFileUser >> strFilePass)) {
			setColor(RED);
			cout<<"LOI HE THONG: File "<<FILE_ADMIN<<" rong hoac sai dinh dang!"<<endl;
			setColor(WHITE);
            fin.close();
            return false;
		}
		fin.close();


		int iAttempts = 0;
		string strInputUser, strInputPass;

		while(iAttempts < 3) {
			setColor(YELLOW);
			cout<<"***************************"<<endl;
        	cout<<"* DANG NHAP ADMIN         *"<<endl;
        	cout<<"***************************"<<endl;
			setColor(WHITE);

			do {
        		cout<<"User: ";
        		getline(cin, strInputUser);
				strInputUser = trim(strInputUser);
				if(strInputUser.empty()) {
					setColor(RED);
					cout<<"Username khong duoc de trong"<<endl;
				}
			} while(strInputUser.empty());

			cout<<"Pin: ";
        	strInputPass = getHiddenPassword();

			if(trim(strInputUser) == strFileUser && strInputPass == strFilePass) {
            	setColor(GREEN);
            	cout<<"Dang nhap Admin thanh cong"<<endl;
            	setColor(WHITE);
            	return true;
        	} else {
				iAttempts++;
				setColor(RED);
				cout<<"Sai thong tin dang nhap"<<endl;
				if(iAttempts < 3) {
					cout<<"Ban con "<<(3 - iAttempts)<<" lan thu"<<endl;
				}
				setColor(WHITE);
			}
		}

		setColor(RED);
		cout<<"\n========================================="<<endl;
		cout<<"  CANH BAO: BAN DA NHAP SAI 3 LAN"<<endl;
        cout<<"  HE THONG TU CHOI TRUY CAP ADMIN"<<endl;
        cout<<"========================================="<<endl;
        setColor(WHITE);
        return false;
	}

	/**********************************************************
    * @Description Hiển thị và xử lý Menu Admin
    *
    * @return Không có
    * @attention Bao gồm các chức năng CRUD nhân viên
    **********************************************************/
	void showAdminMenu() {
		if (!adminLogin()) {
			return;
		}
		int iChoice;
		do {
			setColor(CYAN);
			cout<<"*********MENU**********"<<endl;
			cout<<"    1. Them Employee"<<endl;
			cout<<"    2. Xoa Employee"<<endl;
			cout<<"    3. Tim Employee"<<endl;
			cout<<"    4. Cap nhat Employee"<<endl;
			cout<<"    5. Hien thi thong tin Emplyee"<<endl;
			cout<<"    6. Undo xoa gan nhat"<<endl;
			cout<<"    7. Thoat"<<endl;
			setColor(CYAN);
			cout<<"***********************"<<endl;
			setColor(WHITE);

			iChoice = getInput<int>("Nhap lua chon (1-7): ");

			switch(iChoice) {
				case 1: {
					string strUser, strName, strAddr, strPhone, strEmail;
					setColor(YELLOW);
					cout<<"--- THEM NHAN VIEN ---"<<endl;
					setColor(WHITE);

					do {
						cout<<"Nhap Username: ";
						getline(cin, strUser);
						strUser = trim(strUser);

						if(strUser.empty()) {
							setColor(RED);
							cout<<"Username khong duoc de trong"<<endl;
							setColor(WHITE);
							continue;
						}

						if(!isValidUsernameFormat(strUser)) {
							setColor(RED); 
							cout<<"Username khong dung dinh dang (khong chua dau cach, ky tu dac biet va toi thieu 3 ky tu)"<<endl;
							setColor(WHITE);
							strUser = ""; 
							continue;
						}

						if(_manager.isUsernameExist(trim(strUser))) {
							setColor(RED);
							cout<<"Username da ton tai, vui long nhap lai"<<endl;
							setColor(WHITE); 
							strUser = "";
						}
					} while(strUser.empty());

					do {
						cout<<"Nhap ho ten: ";
						getline(cin, strName);
						strName = trim(strName);
						if(strName.empty()) {
							setColor(RED);
							cout<<"Ho ten khong duoc de trong"<<endl;
							setColor(WHITE);
							break;
						} else if(!isValidName(strName)) {
							setColor(RED);
							cout<<"Ten khong duoc chua so hoac ky tu dac biet"<<endl;
							setColor(WHITE);
							strName = "";
						}
					} while(strName.empty()); 

					do {
						cout<<"Nhap dia chi (toi thieu 5 ky tu): ";
    					getline(cin, strAddr);
    					strAddr = trim(strAddr);
    					if(strAddr.length() < 5) {
        					setColor(RED); 
							cout<<"Dia chi qua ngan, vui long nhap lai"<<endl; 
							setColor(WHITE);
    					}
					} while(strAddr.length() < 5);

					do {
						cout<<"Nhap SDT (10 so): ";
						getline(cin, strPhone);
						if(!isValidPhoneNumber(trim(strPhone))) {
							setColor(RED);
							cout<<"So dien thoai phai co dung 10 chu so"<<endl;
							setColor(WHITE);
						}
					}while(!isValidPhoneNumber(trim(strPhone)));

					do {
						cout<<"Nhap Email (phai co duoi @gmail.com): ";
						getline(cin, strEmail);

						strEmail = trim(strEmail);

						if(!isValidGmail(strEmail)) {
							setColor(RED);
							cout<<"Email khong hop le"<<endl;
							setColor(WHITE);
						} 
					} while(!isValidGmail(strEmail));

					_manager.addEmployee(new Employee(trim(strUser), DEFAULT_PASS, standardizeNames(strName), strAddr, strPhone, strEmail));
					setColor(GREEN); 
					cout << "Them thanh cong! Mat khau mac dinh la " << DEFAULT_PASS << "\n"; setColor(WHITE);
					break;
				}
				case 2: {
					string strUser; 
					cout<<"Nhap username can xoa: "; 
					getline(cin, strUser);
					_manager.deleteEmployee(trim(strUser));
					break;
				}
				case 3: {
					string strUser;
					cout<<"Nhap username can tim: ";
					getline(cin, strUser);
					Employee* pEmp = _manager.getEmployeeByUsername(trim(strUser));
					if(pEmp) {
						setColor(GREEN);
						cout<<"Tim thay: "<<endl;
						setColor(CYAN);
						cout<<left<<setw(15)<<"USERNAME"<<setw(25)<<"HO TEN"<<setw(15)<<"SDT"<<setw(25)<<"EMAIL"<<setw(20)<<"DIA CHI"<<endl;
						cout << string(100, '-') << endl;
						setColor(WHITE);
						cout << *pEmp << endl;
					} else {
						setColor(RED);
						cout<<"Khong tim thay"<<endl; 
						setColor(WHITE);
					}
					break;
				}
				case 4: {
					_manager.updateEmployeeInfo();
					break;
				}
				case 5:
					_manager.showAll(); 
                    break;
				case 6:
					_manager.undoDelete();
					break;
				case 7:
					setColor(YELLOW);
					cout<<"Dang xuat Admin..."<<endl;
					setColor(WHITE);
					break;
				default:
				setColor(RED);
				cout<<"Lua chon khong hop le"<<endl;
				setColor(WHITE);
			} 
		} while(iChoice != 7);
	}

	/**********************************************************
    * @Description Đổi mật khẩu cho Employee
    *
    * @param pEmp Pointer nhân viên cần đổi pass
    * @param bForced true nếu là lần đầu đăng nhập (ép buộc đổi)
    * @return Không có
    **********************************************************/
	void changePass(Employee* pEmp, bool bForced) {
		string strOld, strNew, strConfirm;
		if(bForced) {
			setColor(YELLOW);
			cout<<"\n========================================="<<endl;
			cout<<"BAN DANG NHAP LAN DAU VUI LONG DOI MAT KHAU"<<endl;
			cout<<"========================================="<<endl;
			setColor(WHITE);
		} else {
			cout<<"Nhap mat khau cu: ";
			strOld = getHiddenPassword();
			if(strOld != pEmp->getPassword()) {
				setColor(RED);
				cout<<"Mat khau cu khong dung"<<endl;
				setColor(WHITE);
				return;
			}
		}

		while(true) {
			cout<<"Nhap mat khau moi: ";
			strNew = getHiddenPassword();
			if(strNew.empty()) {
				setColor(RED);
				cout<<"Mat khau khong duoc de trong";
				setColor(WHITE);
				continue;
			}
			if(strNew.length() < 6) {
				setColor(RED);
				cout<<"Mat khau phai co it nhat 6 ky tu"<<endl;
				setColor(WHITE);
				continue;
			}

			cout<<"Xac nhan mat khau moi: ";
			strConfirm = getHiddenPassword();

			if(strNew == strConfirm) {
				pEmp->setPassword(strNew);
                _manager.saveToFile();
				setColor(GREEN);
				cout<<"Doi mat khau thanh cong"<<endl;
				setColor(WHITE);
				break;
			} else {
				setColor(RED);
				cout<<"Mat khau xac nhan khong khop, vui long thu lai"<<endl;
				setColor(WHITE);
			}
		}
	} 

	/**********************************************************
    * @Description Hiển thị và xử lý Menu Employee
    *
    * @return Không có
    * @attention Cho phép thử sai 3 lần trước khi khóa
    **********************************************************/
	void showEmployeeMenu() {
		int iAttempts = 0;
		while(iAttempts < 3) {
			string strUser, strPass;
			cout<<"\n***************************"<<endl;
			cout<<"*  DANG NHAP EMPLOYEE      *"<<endl;
			cout<<"***************************"<<endl;
			setColor(WHITE);
			cout<<"User: ";
			getline(cin, strUser);
			cout<<"Pass: ";
			strPass = getHiddenPassword();

			_pCurrentUser = _manager.login(trim(strUser), strPass);

			if(_pCurrentUser) {
				setColor(GREEN);
				cout<<"Xin chao, "<< _pCurrentUser->getName()<<endl;
				setColor(WHITE);
				if(_pCurrentUser->getPassword() == DEFAULT_PASS) {
					changePass(_pCurrentUser, true);
				}

				int iChoice;
				do {
					setColor(CYAN);
					cout<<"*******MENU EMPLOYEES*******"<<endl;
					cout<<"     1. Xem thong tin tai khoan"<<endl;
					cout<<"     2. Doi password"<<endl;
					cout<<"     3. Thoat"<<endl;
					setColor(CYAN);
					cout<<"****************************"<<endl;
					setColor(WHITE);

					iChoice = getInput<int>("Nhap lua chon: ");

					if(iChoice == 1) {
						_manager.showMyInfoFromFile(_pCurrentUser->getUsername());

					} else if(iChoice == 2) {
						changePass(_pCurrentUser, false);
					} else if(iChoice == 3) {
						setColor(YELLOW);
						cout<<"Dang xuat..."<<endl;
						setColor(WHITE);
					} else {
						setColor(RED);
						cout<<"Lua chon khong hop le"<<endl;
						setColor(WHITE);
					}
				} while(iChoice != 3);

				_pCurrentUser = nullptr;
                return;
			} else {
				setColor(RED);
				cout<<"Sai Username hoac Password"<<endl;
				cout<<"Con "<<(2 - iAttempts)<<" lan thu"<<endl;
				setColor(WHITE);
				iAttempts++;
			}
		}
		setColor(RED);
		cout<<"\n========================================="<<endl;
		cout<<"  BAN DA NHAP SAI 3 LAN"<<endl;
		cout<<"  HE THONG KHOA CHUC NANG DANG NHAP"<<endl;
		cout<<"========================================="<<endl;
		setColor(WHITE);
	}

public:
	/**********************************************************
    * @Description Hàm chạy chính của ứng dụng (Main Loop)
    *
    * @return Không có
    * @attention Điều hướng người dùng vào Admin hoặc Employee Menu
    **********************************************************/
	void run() {
		ensureDirectoryExists();
		while(true) {
			setColor(YELLOW);
			cout<<"\n============================================="<<endl;
			cout<<"   HE THONG QUAN LY NHAN SU (POPEYES)   "<<endl;
			cout<<"============================================="<<endl;
			setColor(WHITE);
			cout<<"1. Admin (Quan tri vien)"<<endl;
            cout<<"2. Employee (Nhan vien)"<<endl;
            cout<<"3. Thoat chuong trinh"<<endl;
			setColor(YELLOW);
			cout<<"============================================="<<endl;
			setColor(WHITE);

			int iRole = getInput<int>("Nhap vai tro cua ban (1-3): ");
			
			if(iRole == 1) {
				showAdminMenu();
			} else if(iRole == 2) {
				showEmployeeMenu();
			} else if(iRole == 3) {
				cout<<"Ket thuc chuong trinh, tam biet"<<endl;
				break;
			} else {
				cout<<"Lua chon khong hop le"<<endl;
			}
		}
	}
};

int main() {
	SetConsoleOutputCP(65001); 
    Application app;
    app.run();
	
return 0;
}
