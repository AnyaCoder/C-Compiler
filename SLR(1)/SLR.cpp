#include <vector>
#include <set>
#include <string>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <map>
#include <stack>
#include <cstdlib> // 命令行程序
#include <iostream>
#include <fstream>
#include <sstream>

// 产生式定义 
struct Production {
    char head; // 箭头左边（非终结符）
    std::string body; // 箭头右边 
    std::string sematic; // 语义动作
};

typedef struct Token {
    int line;
    int idx;
    char type;
    std::string ori; // 原本的标识符
}Token;

// 符号类型
enum TokenType {
    S, L, LPrime, APrime, E, EPrime, T, TPrime, F, A, M, V,
    IF = 12, THEN, WHILE, DO, BEGIN, END, SEMICOLON,
    ASSIGN, L_BRANKET, R_BRANKET, VAR, PLUS, MINUS, MUL, DIV, EOS, UNKNOWN
};

void idx_to_token(struct Token* pToken);

// 一个项目
struct Item {
    Production prod;
    int dotPos; // ‘・’的位置

    bool operator<(const Item& other) const {
        return (prod.head < other.prod.head) ||
            (prod.head == other.prod.head && prod.body < other.prod.body) ||
            (prod.head == other.prod.head && prod.body == other.prod.body && dotPos < other.dotPos);
    }

    bool operator==(const Item& other) const {
        return prod.head == other.prod.head && prod.body == other.prod.body;
    }
};

// 自定义的栈
template<typename T>
class Stack {
private:
    std::vector<T> elements;

public:
    // 判断栈是否为空
    bool isEmpty() const {
        return elements.empty();
    }

    // 压栈
    void push(const T& elem) {
        elements.push_back(elem);
    }

    // 弹栈
    void pop() {
        if (!elements.empty()) {
            elements.pop_back();
        }
    }

    // 获取栈顶元素
    T top() const {
        if (!elements.empty()) {
            return elements.back();
        }
        throw std::runtime_error("尝试访问空栈");
    }

  
    std::string toString() const {
        std::stringstream ss;
        for (auto it = elements.begin(); it != elements.end(); ++it) {
            ss << *it << " ";
        }
        return ss.str();
    }
};

// 求项目集闭包
std::set<Item> Closure(std::set<Item> items, const std::vector<Production>& prods);

// 求Goto集
std::set<Item> Goto(const std::set<Item>& items, char symbol, const std::vector<Production>& prods);

// 打印项目集
void printItemSet(const std::set<Item>& closureSet);

// 判断是否为终结符
bool isTerminal(char symbol) {
    return !isupper(symbol);
}

// 计算FOLLOW集
std::set<char> FOLLOW(
    char nonTerminal,
    const std::vector<Production>& productions,
    std::map<char, std::set<char>>& followSets);


// 伪代码描述
std::map<std::pair<int, char>, std::string> ACTION;
std::map<std::pair<int, char>, int> GOTO;

// 构建ACTION表
void buildACTIONTable(
    const std::vector<std::set<Item>>& calcSet,
    const std::vector<Production>& productions,
    std::map<char, std::set<char>>& followSets
);

// 构建GOTO表
void buildGOTOTable(
    const std::vector<std::set<Item>>& calcSet,
    const std::vector<Production>& productions,
    const std::set<char>& nonTerminals
);

// SLR文法解析器
void SLRParser(const std::vector<Token>& input, const std::vector<Production>& prods);

// 四元式结构
struct Quadruple {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
    Quadruple() {}
    Quadruple(std::string _op, std::string _arg1
        ,std::string _arg2, std::string _result) :
        op(_op), arg1(_arg1), arg2(_arg2), result(_result) {}
    friend std::ostream& operator << (std::ostream& os, const Quadruple& rhs) {
        os << "(" << rhs.op << ", " << rhs.arg1 << ", " << rhs.arg2 << ", " << rhs.result << ")";
        return os;
    }
};

int temp_name_count = 0;
// 用于生成临时变量名的函数
std::string generateTemp() {
    return "t" + std::to_string(temp_name_count++);
}

// 去除字符串左右空白字符
std::string trim(const std::string& str) {
    
    auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos)
        return ""; 

    auto end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

Quadruple gen_quad(const std::string& str_tmp, const std::vector<std::string>& word_tmp) {
    int len = (int)str_tmp.length();
    switch (len)
    {
    case 1: // V -> i
        return Quadruple("_", "_", "_", word_tmp[0]);
    case 3:
        if (str_tmp[0] == '(') { // （E）
            return Quadruple("_", "_", "_", word_tmp[1]);
        }
        else if (str_tmp[1] == '=') { // V=E
            return Quadruple("=", word_tmp[2], "_", word_tmp[0]);
        }
        else {
            std::string tmp_name = generateTemp(); //T1=A+B
            return Quadruple(word_tmp[1], word_tmp[0], word_tmp[2], tmp_name);
        }
    default:
        return Quadruple("_", "_", "_", "_");
    }
}
int main() {
    // 定义产生式集合
    std::vector<Production> prods = {
        {'X', "S", "null"},
        {'S', "V=E", "V.val=E.val"},
        {'E', "E+T", "E.val=E1.val+T.val"},
        {'E', "E-T", "E.val=E1.val-T.val"},
        {'E', "T", "E.val=T.val"},
        {'T', "T*F", "T.val=T1.val*F.val"},
        {'T', "T/F", "T.val=T1.val/F.val"},
        {'T', "F", "T.val=F.val"},
        {'F', "(E)", "F.val=E.val"},
        {'F', "i", "F.val=i.lexval"},
        {'V', "i", "V.val=i.lexval"}
    };

    // 所有字符集
    std::set<char> charSet;
    for (auto& prod : prods) {
        charSet.insert(prod.head);
        for (auto j : prod.body) {
            charSet.insert(j);
        }
    }

    // 非终结符集
    std::set<char> nonTerminalSet;
    for (auto& prod : prods) {
        nonTerminalSet.insert(prod.head);
    }

    // 终结符集
    std::set<char> TerminalSet;
    TerminalSet.insert('$');
    for (auto& prod : prods) {
        for (char ch : prod.body) {
            if (isTerminal(ch)) {
                TerminalSet.insert(ch);
            }
        }
    }

    std::map<char, std::set<char>> followSets;
    std::cout << "============ Follow集： ==========" << "\n";
    for (auto& nt : nonTerminalSet) {
        std::set<char> ntFollow = FOLLOW(nt, prods, followSets);
        std::cout << "非终结符: " << nt << "\n";
        for (char symbol : ntFollow) {
            std::cout << symbol << ' ';
        }
        std::cout << "\n";
    }
    // 初始项目集
    std::set<Item> initialSet = { { *prods.begin(), 0}};

    // 创建一个队列
    std::queue<std::set<Item>> itemSetQueue;
    itemSetQueue.push(initialSet);

    int count = 0;

    std::vector<std::set<Item>> calcSet;

    std::cout << "============该SLR文法的项目集族: ===============" << std::endl;
    while (!itemSetQueue.empty()) {
        auto topSet = itemSetQueue.front();
        itemSetQueue.pop();
        // 计算闭包
        
        std::set<Item> closureSet = Closure(topSet, prods);

        bool gotoNext = false;
        for (const auto& i : calcSet) {
            if (closureSet == i) {
                gotoNext = true;
                break;
            }
        }
        if (gotoNext) {
            continue;
        }
        std::cout << "I(" << count++ << "):" << std::endl;
        printItemSet(closureSet);
        calcSet.push_back(closureSet);
        // 应用 GOTO 函数
        for (char symbol : charSet) {
            std::set<Item> gotoSet = Goto(closureSet, symbol, prods);
            if (!gotoSet.empty()) {
                itemSetQueue.push(gotoSet);
            }
            
        }

    }
    
    buildACTIONTable(calcSet, prods, followSets);
    buildGOTOTable(calcSet, prods, nonTerminalSet);
   
    std::cout << "\n\n";
    std::cout << "========= ACTION表和GOTO表: ===========" << std::endl;
    std::cout << std::setw(5) << " ";
    for (char t : TerminalSet) {
        std::cout << std::setw(4) << t << " ";
    }
    std::cout << "|";
    for (char nt : nonTerminalSet) {
        std::cout << std::setw(4) << nt << " ";
    }
    std::cout << "\n";

    // 打印分隔行
    for (int i = 0; i < (int)(TerminalSet.size() + nonTerminalSet.size() + 1) * 5; i++) {
        std::cout << "-";
    }
    std::cout << "\n";

    for (int i = 0; i < (int)calcSet.size(); i++) {
        std::cout << std::setw(4) << i << " ";
        for (char t : TerminalSet) {
            std::string action = ACTION[{i, t}];
            std::cout << std::setw(4) << action << " ";
        }
        std::cout << "|";
        for (char nt : nonTerminalSet) {
            int action = GOTO[{i, nt}];
            std::cout << std::setw(4) << (action == 0 ? " " : std::to_string(action)) << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n 对应的产生式： " << std::endl;
    for (int i = 0; i < (int)prods.size(); i++) {
        std::cout << "P(" << i << ") :" << prods[i].head << " -> " << prods[i].body << "\t";
        if (i % 5 == 0) {
            std::cout << std::endl;
        }
    }

    std::cout << "\n\n ========= 测试环节 ========= \n\n" << std::endl;
    std::string in_str;
    while (true) {
        std::cout << std::setw(0);
        std::cout << "请输入一个表达式串以供解析， 输入 'exit' 退出（不含引号)" << std::endl;
        std::getline(std::cin, in_str);
        if (in_str == "exit") {
            break;
        }
       
        std::ofstream outfile("1.vits");
        if (!outfile.is_open()) {
            std::cerr << "无法打开文件" << std::endl;
            return 1;
        }

        outfile << in_str;

        outfile.close();

        std::cout << "\n字符串已成功写入文件! 文件内容如下：" << std::endl;
        system("type 1.vits");
        std::cout << "\n-------------------\n";
        system("..\\x64\\Debug\\Lexeme.exe 1.vits 1.txt");

        std::ifstream inputFile("1.txt");
        if (!inputFile.is_open()) {
            std::cerr << "无法打开文件" << std::endl;
            return 1;
        }

        // 逐行读取文件内容
        std::string line;
        std::vector<Token> tokens;
        while (std::getline(inputFile, line)) {
            // 使用逗号作为分隔符拆分每一行
            std::istringstream iss(line);
            std::string str_token;
            std::vector<std::string> str_tokens;
            while (std::getline(iss, str_token, ',')) {
                str_tokens.push_back(str_token);
            }
            str_tokens[2] = trim(str_tokens[2]);
            Token new_token = Token{
                // 行数， 编号， 符号类型， 原符号
                std::stoi(str_tokens[0]), std::stoi(str_tokens[1]), '_', str_tokens[2]
            };
            idx_to_token(&new_token);
            tokens.push_back(new_token);
        }

        inputFile.close();
        temp_name_count = 0; // 清零计数器
        SLRParser(tokens, prods);
    }
    return 0;
}


void SLRParser(const std::vector<Token>& input, const std::vector<Production>& prods) {
    // 初始化栈
    Stack<int> stateStack;
    Stack<char> symbolStack;
    Stack<std::string> wordStack;
    std::vector<Quadruple> quadruples;  // 存储生成的四元式
    // 初始状态
    stateStack.push(0);
    symbolStack.push('$');
    wordStack.push("$");
    std::vector<Token> buffer = input;
    buffer.push_back(Token{0, 0, '$', "$"});
    int inputPos = 0;
    int step = 0;  // 初始化步骤计数器

    std::cout << std::left
        << std::setw(5) << "步骤" 
        << std::setw(30) << "符号栈"
        << std::setw(30) << "状态栈" 
        << std::setw(20) << "输入串" 
        << std::setw(30) << "动作" << "\n\n";

   
    while (true) {
        int currentState = stateStack.top();
        char currentSymbol = buffer[inputPos].type;
        std::string currentWord = buffer[inputPos].ori;
        std::string ss;

        // 获取当前状态和符号对应的动作
        std::string action = ACTION[{currentState, currentSymbol}];

        std::cout << std::left << std::setw(5) << step;
        std::cout << std::setw(30) << symbolStack.toString();
        std::cout << std::setw(30) << stateStack.toString();
        std::cout << std::setw(20);
        for (int i = inputPos; i < (int)buffer.size(); i++) {
            ss += buffer[i].type;
        }
        std::cout << ss;
        std::cout << std::setw(30);
        ss = "";
        if (action.empty()) {
            ss += "错误:无有效动作。\n";
            std::cout << ss;
            break;
        }

        // 执行动作
        if (action[0] == 's') {  // 移入操作
            // 处理移入
            symbolStack.push(currentSymbol);
            wordStack.push(currentWord);
            stateStack.push(std::stoi(action.substr(1)));
            inputPos++;

            ss += "移入";
            ss += currentSymbol;
            std::cout << ss;
            step++;

        }
        else if (action[0] == 'r') {  // 归约操作
            // 处理规约
            int prodIndex = std::stoi(action.substr(1));
            const Production& prod = prods[prodIndex];
            std::string str_tmp; // 类型
            std::vector<std::string> word_tmp; // 实际标识符
            for (int i = 0; i < prod.body.size(); ++i) {
                str_tmp += symbolStack.top(); // 将退栈内容保存到临时字符串
                word_tmp.push_back(wordStack.top());
                wordStack.pop();
                symbolStack.pop();
                stateStack.pop();
            }
            // 加入符号栈
            symbolStack.push(prod.head);

            // 产生下一状态
            int nextState = GOTO[{stateStack.top(), prod.head}];
            stateStack.push(nextState);         

            std::reverse(str_tmp.begin(), str_tmp.end());
            std::reverse(word_tmp.begin(), word_tmp.end());

            auto quad = gen_quad(str_tmp, word_tmp);
            if (quad.op != "_" && !(quad.op == "=" && quad.arg1 == quad.result)) {
                quadruples.push_back(quad);
            }
            
            wordStack.push(quad.result);      
            // 信息输出
            ss += "用产生式 ";
            ss += prod.head;
            ss += "->";
            ss += prod.body;
            ss += " 来归约";
            std::cout << ss;
            step++;
        }
        else if (action == "acc") {
            ss += "分析成功。\n";
            std::cout << ss << std::endl;
            break;
        }
        else {
            ss += "未知动作。\n";
            std::cout << ss << std::endl;
            break;
        }
        std::cout << "\n" << std::endl;
    }
    std::cout << "产生的四元式序列： " << "\n";
    // 打印生成的四元式
    int cnt = 0;
    for (const auto& q : quadruples) {
        std::cout << ++cnt << " " << q << std::endl;
    }
}

std::set<Item> Closure(std::set<Item> items, const std::vector<Production>& prods) {
    bool added;
    do {
        added = false;
        std::set<Item> newItems;

        for (const auto& item : items) {
            if (item.dotPos < (int)item.prod.body.length()) {
                char symbol = item.prod.body[item.dotPos];

                // 对每个产生式 B → γ，如果符号 B 在点的右边
                for (const auto& prod : prods) {
                    if (prod.head == symbol) {
                        Item newItem = { prod, 0 };
                        if (items.find(newItem) == items.end() && newItems.find(newItem) == newItems.end()) {
                            newItems.insert(newItem);
                            added = true;
                        }
                    }
                }
            }
        }

        items.insert(newItems.begin(), newItems.end());
    } while (added);

    return items;
}

std::set<Item> Goto(const std::set<Item>& items, char symbol, const std::vector<Production>& prods) {
    std::set<Item> resultSet;
    for (const auto& item : items) {
        if (item.dotPos < (int)item.prod.body.length() && item.prod.body[item.dotPos] == symbol) {
            Item newItem = item;
            newItem.dotPos++; // 移动点的位置
            resultSet.insert(newItem);
        }
    }
    return Closure(resultSet, prods); // 对结果集计算闭包
}


std::set<char> FOLLOW(
    char nonTerminal,
    const std::vector<Production>& productions,
    std::map<char, std::set<char>>& followSets) {
    // 如果FOLLOW集已经计算过，则直接返回
    if (followSets.find(nonTerminal) != followSets.end()) {
        return followSets[nonTerminal];
    }

    std::set<char> result;
    if (nonTerminal == 'S') { // 假设'S'是起始符号
        result.insert('$');
    }

    for (const auto& prod : productions) {
        std::string::size_type pos = prod.body.find(nonTerminal);
        while (pos != std::string::npos) {
            if (pos + 1 < prod.body.size()) {
                char nextSymbol = prod.body[pos + 1];
                if (isTerminal(nextSymbol)) {
                    result.insert(nextSymbol);
                }
                else {
                    // 将nextSymbol的FOLLOW集合加入
                    std::set<char> nextFollow = FOLLOW(nextSymbol, productions, followSets);
                    result.insert(nextFollow.begin(), nextFollow.end());
                }
            }
            else if (prod.head != nonTerminal) {
                // 规则的末尾，将左侧符号的FOLLOW集加入
                std::set<char> headFollow = FOLLOW(prod.head, productions, followSets);
                result.insert(headFollow.begin(), headFollow.end());
            }
            pos = prod.body.find(nonTerminal, pos + 1);
        }
    }

    followSets[nonTerminal] = result;
    return result;
}

void buildACTIONTable(
    const std::vector<std::set<Item>>& calcSet,
    const std::vector<Production>& productions,
    std::map<char, std::set<char>>& followSets
) {
    for (int i = 0; i < (int)calcSet.size(); ++i) {
        for (const Item& item : calcSet[i]) {
            if (item.dotPos < item.prod.body.size()) {
                char symbol = item.prod.body[item.dotPos];
                if (isTerminal(symbol)) {
                    // 移入操作
                    for (int j = 0; j < (int)calcSet.size(); ++j) {
                        if (calcSet[j] == Goto(calcSet[i], symbol, productions)) {
                            ACTION[{i, symbol}] = "s" + std::to_string(j);
                            break;
                        }
                    }
                }
            }
            else {
                // 规约操作
                if (item.prod.head != 'X') { // 'X' 是扩展的起始符号
                    for (char terminal : followSets[item.prod.head]) {
                        // 查找产生式编号
                        int prodIndex = -1;
                        for (int k = 0; k < (int)productions.size(); ++k) {
                            if (productions[k].head == item.prod.head && productions[k].body == item.prod.body) {
                                prodIndex = k;
                                break;
                            }
                        }
                        if (prodIndex != -1) {
                            ACTION[{i, terminal}] = "r" + std::to_string(prodIndex);
                        }
                    }
                }
                else {
                    // 接受操作
                    ACTION[{i, '$'}] = "acc";
                }
            }
        }
    }
}

// 构建GOTO表
void buildGOTOTable(
    const std::vector<std::set<Item>>& calcSet,
    const std::vector<Production>& productions,
    const std::set<char>& nonTerminals
) {
    for (int i = 0; i < (int)calcSet.size(); ++i) {
        for (char symbol : nonTerminals) { // 假设nonTerminals是所有非终结符的集合
            for (int j = 0; j < (int)calcSet.size(); ++j) {
                if (calcSet[j] == Goto(calcSet[i], symbol, productions)) {
                    GOTO[{i, symbol}] = j;
                    break;
                }
            }
        }
    }
}


void idx_to_token(struct Token* pToken) {
    switch ((*pToken).idx)
    {

    case 32:
        (*pToken).type = 'i';
        break;

    case 34:
        (*pToken).type = '+';
        break;
    case 35:
        (*pToken).type = '-';
        break;
    case 36:
        (*pToken).type = '=';
        break;
    case 42:
        (*pToken).type = '*';
        break;
    case 43:
        (*pToken).type = '/';
        break;
    case 52:
        (*pToken).type = '(';
        break;
    case 53:
        (*pToken).type = ')';
        break;
    default:
        (*pToken).type = UNKNOWN;
        break;
    }
}

void printItemSet(const std::set<Item>& closureSet) {
    // 打印结果
    for (const auto& item : closureSet) {
        std::cout << item.prod.head << " -> ";
        int len = (int)item.prod.body.length();
        for (int i = 0; i < len; i++) {
            if (i == item.dotPos) {
                std::cout << "·";
            }
            std::cout << item.prod.body[i];
        }
        if (len == item.dotPos) {
            std::cout << "·";
        }
        std::cout << std::endl;
    }
}
