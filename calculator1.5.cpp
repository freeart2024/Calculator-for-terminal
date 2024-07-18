#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <stack>
#include <unordered_set>
#include <sstream>
#include <array>
#include "log.h"
using namespace std;
// 定义计算器类
class calculator {
public:
    double output = 0.0; // 存储计算结果的成员变量

    calculator() {}; // 默认构造函数
    // 带有一个字符串参数的构造函数，用于初始化表达式
    calculator(std::string a) :expression(a){}
    inline ~calculator() {}; // 析构函数

    // 计算表达式的函数
    bool evaluate(std::string& a) {
        // 检查表达式的首尾字符是否为数字
        string error;
        if (!isdigit(expression[0]) || !isdigit(expression[expression.size() - 1])) {
            if (expression[0] != '(' && expression[expression.size() - 1] != ')') {
                error = "错误：第一个字符和最后一个字符不能是运算符。";
                log(error, 2);
                return true;
            }
        }

        // 检查表达式中是否含有非法字符
        for (char i : expression) {
            if (find(pass_str.begin(), pass_str.end(), i) == pass_str.end()) {
                stringstream ss;
                ss << "错误：没有叫“" << i << "”的运算符。";
                error = ss.str();
                log(error, 2);
                return true;
            }
        }
        if (!isValid(expression)) {
            error = "警告：未闭合的括号。";
            log(error, 1);
            return true;
        }
        std::stack<char> op_stack; // 操作符栈  
        op_stack.push('\0'); // 向栈中压入一个空字符作为哨兵  

        std::stack<double> value; // 操作数栈  
        std::string Postfix = infixToPostfix(expression); // 将中缀表达式转为后缀表达式  
        a = Postfix;
        // 直接遍历后缀表达式并计算，不需要split函数  
        for (size_t i = 0; i < Postfix.size(); ++i) {
            if (isdigit(Postfix[i]) || Postfix[i] == '.') {
                // 处理多位数和小数  
                std::string num_str;
                while (i < Postfix.size() && (isdigit(Postfix[i]) || Postfix[i] == '.')) {
                    num_str += Postfix[i++];
                }
                --i; // 因为for循环中的i已经自增，这里需要回退  
                value.push(stod(num_str)); // 将数字字符串转为数字并入栈  
            }
            else if (find(pass.begin(), pass.end(), std::string(1, Postfix[i])) != pass.end()) {
                // 如果是操作符  
                double b = value.top(); value.pop(); // 弹出第二个操作数（后缀表达式中在操作符前的操作数）  
                double a = value.top(); value.pop(); // 弹出第一个操作数（即紧挨着操作符的操作数）  
                try {
                    output = apply_op(a, std::string(1, Postfix[i]), b); // 注意操作数的顺序  
                    value.push(output); // 将计算结果入栈  
                }
                catch (std::runtime_error err) {
                    error = err.what();
                    log(error, 1);
                    return true;
                }
            }
        }
        return false;
    }
private:
    inline static bool isValid(std::string e) {
        std::array<char, 2> p = { '(', ')' };
        std::stack<char> ExStack;
        for (char i : e) {
            if (i == p[0]) {
                ExStack.push(i);
            }
            else if (i == p[1]) {
                if (ExStack.empty()) {
                    return false;
                }
                else {
                    ExStack.pop();
                }
            }
        }return ExStack.empty();
    }
    inline bool isnum(const std::string& a) const {
        try {
            int b = stod(a);
        }
        catch (invalid_argument err) {
            return false;
        }catch (out_of_range err) {
            return false;
        }return true;
    }
    std::string expression; // 存储表达式的成员变量

    // 执行计算操作的函数
    inline double apply_op(double a, string op, double b) const {
        if (op == "+") { return a + b; }
        if (op == "-") { return a - b; }
        if (op == "*") { return a * b; }
        if (op == "/") {
            if (b == 0.0) { throw std::runtime_error("警告：你不能除以零。"); }
            else { return a / b; }
        }
    }
    const std::unordered_set<char> pass_str{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '+', '-', '*', '/', '(', ')', '.' };// 存储所有合法字符的无序集合
    const std::unordered_set<std::string> pass = { "+", "-", "*", "/" };// 存储所有操作符的无序集合
    // 将中缀表达式转为后缀表达式
    std::unordered_map<char, int> op_precedence = { {'+', 1},{'-', 1},{'*', 2},{'/', 2},{'(', 0} };
    inline int precedence(char op) const {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return 0;
    }

    std::string infixToPostfix(const std::string& infix) {
        std::stack<char> opStack;
        std::ostringstream postfix;
        for (size_t i = 0; i < infix.size(); ++i) {
            if (isdigit(infix[i]) || (infix[i] == '.' && (i == 0 || !isdigit(infix[i - 1])))) {
                size_t j = i;
                // 读取多位数（包括小数）  
                while (i < infix.size() && (isdigit(infix[i]) || infix[i] == '.')) {
                    ++i;
                }
                --i; // 因为for循环还会自增，所以这里先减一  
                postfix << infix.substr(j, i - j + 1) << " ";
            }
            else if (infix[i] == '(') {
                opStack.push(infix[i]);
            }
            else if (infix[i] == ')') {
                while (!opStack.empty() && opStack.top() != '(') {
                    postfix << opStack.top() << " ";
                    opStack.pop();
                }
                opStack.pop(); // 弹出'('  
            }
            else if (infix[i] == '+' || infix[i] == '-' || infix[i] == '*' || infix[i] == '/') {
                while (!opStack.empty() && precedence(opStack.top()) >= precedence(infix[i])) {
                    postfix << opStack.top() << " ";
                    opStack.pop();
                }
                opStack.push(infix[i]);
            }
            else if (!isspace(infix[i])) {
                // 处理无效字符  
                std::cerr << "中缀表达式中的无效字符：" << infix[i] << std::endl;
                return "";
            }
        }

        while (!opStack.empty()) {
            postfix << opStack.top() << " ";
            opStack.pop();
        }

        return postfix.str();
    }
};
int main() {
    // 输出欢迎信息
    std::cout << "欢迎使用计算器！" << std::endl;
    std::cout << "版本号：V1.1.9" << std::endl;
    std::string a;
    while (true) {
        // 提示用户输入表达式
        std::cout << "输入一个算式:";
        std::getline(std::cin, a);
        // 如果用户输入'0'，则退出程序
        if (a == "0") {
            return 0;
        }
        else if (a == "version") {
            std::cout << "V0.0.1：不能使用" << std::endl;
            std::cout << "V1.1.0：支持一位数的加减乘除，不支持小括号" << std::endl;
            std::cout << "V1.1.2：优化错误提示，更适合中国人使用" << std::endl;
            std::cout << "V1.1.5：支持多位数和小括号" << std::endl;
            std::cout << "V1.1.7：支持浮点数" << std::endl;
            std::cout << "V1.1.9：增加日志输出，更新历史，未闭合的括号错误提示" << std::endl;
            std::cout << "V1.1.92（最新版）:优化代码并优化输出日志，支持带颜色的输出" << std::endl;
            std::cout << "V2.0.1：尽请期待" << std::endl;
            continue;
        }
        // 创建一个计算器对象，并传入用户输入的表达式
        std::unique_ptr<calculator> c = std::make_unique<calculator>(a);
        std::string b;
        if (c->evaluate(b)) {
            continue;
        }
        else {
#ifndef NDEBUG
            log(b, 0);
#endif
            std::cout << "答案:" << c->output << std::endl;
        }
    }
}

