#include <iostream>
#include <string>
#include <list>

using namespace std;

class Big {
    list<int> digits;
    int sign;
    
    void processString(const string& toProcess);
    void removeTrailing0s();

public:
    Big();
    Big(int val);
    ~Big();
    int abs_max(const Big& other);
    Big operator+(const Big& other);
    void operator+=(const Big& other);
    Big operator-(const Big& other);
    Big operator*(const Big& other);
    friend istream& operator>>(istream& input, Big& number);
    friend ostream& operator<<(ostream& output, const Big& number);
    friend class BigVector;
};

class BigVector {
    list<Big> columns;

public:
    ~BigVector();
    Big abs_max();
    Big operator*(const BigVector& other);
    friend istream& operator>>(istream& input, BigVector& vector);
    friend ostream& operator<<(ostream& output, const BigVector& vector);
};

Big::Big() {
    sign = 1;
}

Big::Big(int val) {
    string insert = to_string(val);
    this->processString(insert);
}

Big::~Big() {
    this->digits.clear();
}

BigVector::~BigVector() {
    this->columns.clear();
}

void Big::processString(const string& toProcess) {
    string::const_iterator poz;

    poz = toProcess.begin();

    if (toProcess[0] == '-') {
        this->sign = -1;
        poz++;
    }
    else {
        this->sign = 1;
    }
    while (poz != toProcess.end() && *poz == '0') {
        poz++;
    }
    if (poz == toProcess.end()) {
        this->digits.push_front(0);
        this->sign = 1;
        return;
    }
    
    for (; poz != toProcess.end(); poz++) {
        this->digits.push_front(*poz - '0');
    }
}

void Big::removeTrailing0s() {
    list<int>::iterator i;
    i = digits.end();
    i--;
    if (*i != 0) {
        return;
    }
    while (i != digits.begin() && *i == 0) {
        i--;
    }
    i++;
    digits.erase(i, digits.end());
    if (++i == digits.begin())
        this->sign = 1;
}

int Big::abs_max(const Big& other) {
    list<int>::iterator a;
    list<int>::const_iterator b;

    a = this->digits.begin();
    b = other.digits.begin();
    while (a != this->digits.end() && b != other.digits.end()) {
        a++;
        b++;
    }
    if (a != this->digits.end())
        return 1;
    if (b != other.digits.end())
        return -1;
    a--;
    b--;
    while (a != this->digits.begin() && b != other.digits.begin() && *a - *b == 0) {
        a--;
        b--;
    }
    return ((*a - *b) < 0) ? -1 : 1;
}

Big BigVector::abs_max() {
    list<Big>::iterator i;
    Big max(0);
    for (i = this->columns.begin(); i != this->columns.end(); i++) {
        max = (i->abs_max(max) == 1) ? *i : max;
    }
    return max;
}

Big Big::operator+(const Big& other) {
    list<int>::iterator a;
    list<int>::const_iterator b;
    int remainder = 0;
    Big result;

    if (this->sign == -1 && other.sign == -1) {
        result.sign = -1;
    }
    else if (this->sign == 1 && other.sign == -1) {
        Big sorryhaveto = other;
        sorryhaveto.sign = 1;
        return *this - sorryhaveto;
    }
    else if (this->sign == -1 && other.sign == 1) {
        Big sorryhaveto = other;
        sorryhaveto.sign = -1;
        return *this - sorryhaveto;
    }

    a = this->digits.begin();
    b = other.digits.begin();
    while (a != this->digits.end() && b != other.digits.end()) {
        result.digits.push_back((*a + *b + remainder) % 10);
        remainder = (*a + *b + remainder) / 10;
        a++;
        b++;
    }
    while (a != this->digits.end()) {
        result.digits.push_back((*a + remainder) % 10);
        remainder = (*a + remainder) / 10;
        a++;
    }
    while (b != other.digits.end()) {
        result.digits.push_back((*b + remainder) % 10);
        remainder = (*b + remainder) / 10;
        b++;
    }
    if (remainder) {
        result.digits.push_back(remainder);
    }

    return result;
}

void Big::operator+=(const Big& other) {
    *this = *this + other;
}

Big Big::operator-(const Big& other) {
    Big result;

    if (this->sign == 1 && other.sign == -1) {
        Big sorryhaveto = other;
        sorryhaveto.sign = 1;
        return *this + sorryhaveto;
    }
    if (this->sign == -1 && other.sign == 1) {
        Big sorryhaveto = other;
        sorryhaveto.sign = -1;
        return *this + sorryhaveto;
    }

    int check1 = this->abs_max(other) == 1;
    int check2 = this->sign == 1;
    const Big& number1 = (check1) ? *this : other;
    const Big& number2 = (check1) ? other : *this;
    result.sign = (check1 == check2) ? 1 : -1;
    
    list<int>::const_iterator a;
    list<int>::const_iterator b;
    int remainder = 0;
    int helper;

    a = number1.digits.begin();
    b = number2.digits.begin();
    while (a != number1.digits.end() && b != number2.digits.end()) {
        helper = *a - *b - remainder;
        if (helper < 0) {
            result.digits.push_back(10 + helper);
            remainder = 1;
        }
        else {
            result.digits.push_back(helper);
            remainder = 0;
        }
        a++;
        b++;
    }
    while (a != number1.digits.end()) {
        helper = *a - remainder;
        if (helper < 0) {
            result.digits.push_back(10 + helper);
            remainder = 1;
        }
        else {
            result.digits.push_back(helper);
            remainder = 0;
        }
        a++;
    }
    
    result.removeTrailing0s();

    return result;

}

Big Big::operator*(const Big& other) {
    Big result(0);
    Big partial_result;
    list<int>::iterator a;
    list<int>::const_iterator b;
    int zeroes = 0;
    int remainder;

    a = this->digits.begin();
    while (a != this->digits.end()) {
        b = other.digits.begin();
        remainder = 0;
        while (b != other.digits.end()) {
            partial_result.digits.push_back((*a * *b + remainder) % 10);
            remainder = (*a * *b + remainder) / 10;
            b++;
        }
        if (remainder) {
            partial_result.digits.push_back(remainder);
        }
        for (int i = 0; i < zeroes; i++) {
            partial_result.digits.push_front(0);
        }
        result += partial_result;
        partial_result.digits.clear();
        a++;
        zeroes++;
    }
    result.sign = this->sign * other.sign;
    
    return result;
}

Big BigVector::operator*(const BigVector& other) {
    Big result(0);
    list<Big>::iterator i = this->columns.begin();
    list<Big>::const_iterator j = other.columns.begin();
    for (; i != this->columns.end(); i++) {
        result += *i * *j;
        j++;
    }
    return result;
}

istream& operator>>(istream& input, Big& number) {
    string read;
    input >> read;
    number.digits.clear();
    number.processString(read);
    return input;
}

ostream& operator<<(ostream& output, const Big& number) {
    list<int>::const_iterator i;
    if (number.sign == -1)
        output << '-';
    i = number.digits.end();
    for (i--; i != number.digits.begin(); i--) {
        output << *i;
    }
    output << *i;
    return output;
}

istream& operator>>(istream& input, BigVector& vector) {
    int number;
    Big insert;
    input >> number;
    vector.columns.clear();
    for (int i = 0; i < number; i++) {
        input >> insert;
        vector.columns.push_back(insert);
    }
    return input;
}

ostream& operator<<(ostream& output, const BigVector& vector) {
    for (list<Big>::const_iterator i = vector.columns.begin(); i != vector.columns.end(); i++) {
        output << *i << ' ';
    }
    return output;
}

int main() {
    Big number1, number2;
    BigVector vecky, becky;
    int read;
    while (1) {
        cout << "1. Suma a doua numere intregi mari" << endl << "2. Diferenta" << endl << "3. Produsul" << endl << "4. Maxim dintre modulele a doua numere" << endl;
        cout << "5. Produsul scalar a doi vector de intregi mari" << endl << "6. Valoarea maxima in modul dintr-un vector" << endl << endl;
        cin >> read;
        switch (read) {
            case 1:
                cout << "Introdu numerele: " << endl << "\t";
                cin >> number1 >> number2;
                cout << "\t\t" << number1 + number2 << endl;
                break;
            case 2:
                cout << "Introdu numerele: " << endl << "\t";
                cin >> number1 >> number2;
                cout << "\t\t" << number1 - number2 << endl;
                break;
            case 3:
                cout << "Introdu numerele: " << endl << "\t";
                cin >> number1 >> number2;
                cout << "\t\t" << number1 * number2 << endl;
                break;
            case 4:
                cout << "Introdu numerele: " << endl << "\t";
                cin >> number1 >> number2;
                cout << "\t\t" << number1.abs_max(number2) << endl;
                cout << "(1 = numarul din stanga are modulul maxim; -1 = numarul din dreapta are modulul maxim)" << endl;
                break;
            case 5:
                cout << "Introdu numarul de elemente, apoi elementele primului vector: " << endl << "\t";
                cin >> vecky;
                cout << "Introdu numarul de elemente, apoi elementele celui de-al doilea vector: " << endl << "\t";
                cin >> becky;
                cout << "\t\t" << vecky * becky << endl;
                break;
            case 6:
                cout << "Introdu numarul de elemente, apoi elementele vectorului: " << endl << "\t";
                cin >> vecky;
                cout << "\t\t" << vecky.abs_max() << endl;
                break;
        }
        cout << endl;
    }
    return 0;
}
