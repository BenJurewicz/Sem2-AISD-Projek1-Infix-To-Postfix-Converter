#include <iostream>
#include <utility>

template<typename T>
class Option {
    T *_value;
    bool _hasValue;
    bool _isValueOwner;
public:
    Option() : _hasValue(false), _isValueOwner(false) {}

    explicit Option(T *value) : _value(value), _hasValue(true), _isValueOwner(false) {}

    explicit Option(T value) : _value(new T(value)), _hasValue(true), _isValueOwner(true) {}

    void setValue(T *value) {
        _value = value;
        _hasValue = true;
        _isValueOwner = false;
    }

    void setValue(T value) {
        _value = new T(value);
        _hasValue = true;
        _isValueOwner = true;
    }

    bool hasValue() const {
        return _hasValue;
    }

    const T *getValue() {
        if (_hasValue) {
            return _value;
        }
        return nullptr;
    }

    ~Option() {
        if (_isValueOwner) {
            delete _value;
        }
    }
};

template<typename T>
class Stack {
    class Node {
    public:
        T *data;
        Node *next;
        Node *previous;

        Node(T *data, Node *next, Node *previous) : data(data), next(next), previous(previous) {}

        ~Node() {
            // Note: in push_back and push_front, instead of copying by value and then deleting data
            // Note: we can just move the data pointer to the return value
            delete data;
        }
    };

    Node *head;
    Node *tail;

public:

    Stack() : head(nullptr), tail(nullptr) {}

    Stack(Stack &&other) noexcept: Stack() {
        this->head = other.head;
        this->tail = other.tail;

        other.head = nullptr;
        other.tail = nullptr;
    }

    Stack(const Stack &other) : Stack() {
        Node *current = other.head;
        while (current != nullptr) {
            push_back(new T(*current->data));
            current = current->next;
        }
    }

    Stack &operator=(Stack &&other) {
        if (this == &other) {
            return *this;
        }
        this->clear();

        this->head = other.head;
        this->tail = other.tail;

        other.head = nullptr;
        other.tail = nullptr;

        return *this;
    }

    Stack &operator=(const Stack &other) {
        if (this == &other) {
            return *this;
        }
        this->clear();

        Node *current = other.head;
        while (current != nullptr) {
            push_back(new T(*current->data));
            current = current->next;
        }

        return *this;
    }

    void push_back(T *val) {
        Node *newNode = new Node(val, nullptr, tail);
        if (head == nullptr && tail == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    void push_front(T *val) {
        Node *newNode = new Node(val, head, nullptr);
        if (head == nullptr && tail == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            head->previous = newNode;
            head = newNode;
        }
    }

    Option<T> pop_back() {
        Option<T> result;
        if (tail == nullptr) {
            return result;
        }
        result.setValue(*tail->data);

        Node *oldTail = tail;
        tail = tail->previous;
        delete oldTail;

        if (tail != nullptr) {
            tail->next = nullptr;
        } else {
            head = nullptr;
        }

        return result;
    }

    Option<T> pop_front() {
        Option<T> result;
        if (head == nullptr) {
            return result;
        }
        result.setValue(*head->data);

        Node *oldHead = head;
        head = head->next;
        delete oldHead;

        if (head != nullptr) {
            head->previous = nullptr;
        } else {
            tail = nullptr;
        }

        return result;
    }

    Option<T> peek_back() const {
        Option<T> result;
        if (tail == nullptr) {
            return result;
        }
        result.setValue(tail->data);
        return result;
    }

    Option<T> peek_front() const {
        Option<T> result;
        if (head == nullptr) {
            return result;
        }
        result.setValue(head->data);
        return result;
    }

    size_t len() const {
        size_t length = 0;
        Node *current = head;
        while (current != nullptr) {
            length++;
            current = current->next;
        }
        return length;
    }

    bool isEmpty() const {
        return head == nullptr && tail == nullptr;
    }

    /**
     * @brief Deletes all elements in the stack
     */
    void clear() {
        while (head != nullptr) {
            pop_back();
        }
    }

    ~Stack() {
        Node *current = head;
        while (current != nullptr) {
            Node *next = current->next;
            delete current;
            current = next;
        }
    }
};

namespace Operations {
    void add(Stack<int> &stack) {
        Option<int> val1 = stack.pop_back();
        Option<int> val2 = stack.pop_back();
        if (val1.hasValue() && val2.hasValue()) {
            stack.push_back(new int(*val1.getValue() + *val2.getValue()));
        }
    }

    void sub(Stack<int> &stack) {
        Option<int> val1 = stack.pop_back();
        Option<int> val2 = stack.pop_back();
        if (val1.hasValue() && val2.hasValue()) {
            stack.push_back(new int(*val1.getValue() - *val2.getValue()));
        }
    }

    void mul(Stack<int> &stack) {
        Option<int> val1 = stack.pop_back();
        Option<int> val2 = stack.pop_back();
        if (val1.hasValue() && val2.hasValue()) {
            stack.push_back(new int(*val1.getValue() * *val2.getValue()));
        }
    }

    void div(Stack<int> &stack) {
        Option<int> val1 = stack.pop_back();
        Option<int> val2 = stack.pop_back();
        if (val1.hasValue() && val2.hasValue()) {
            stack.push_back(new int(*val1.getValue() / *val2.getValue()));
        }
    }

    void neg(Stack<int> &stack) {
        Option<int> val = stack.pop_back();
        if (val.hasValue()) {
            stack.push_back(new int(-*val.getValue()));
        }
    }

    void ifFunc(Stack<int> &stack) {
        Option<int> val1 = stack.pop_back();
        Option<int> val2 = stack.pop_back();
        Option<int> val3 = stack.pop_back();
        if (val1.hasValue() && val2.hasValue() && val3.hasValue()) {
            stack.push_back(new int(*val1.getValue() > 0 ? *val2.getValue() : *val3.getValue()));
        }
    }

    // functions below need argument count to be pushed to stack before evaluation
    void max(Stack<int> &stack) {
        Option<int> count = stack.pop_back();
        if (!count.hasValue()) {
            return;
        }

        int countValue = *count.getValue();
        int currentMax = INT32_MIN;

        for (int i = 0; i < countValue; i++) {
            Option<int> numOption = stack.pop_back();
            if (!numOption.hasValue()) {
                return;
            }
            int num = *numOption.getValue();
            if (currentMax < num) {
                currentMax = num;
            }
        }

        stack.push_back(new int(currentMax));
    }

    void min(Stack<int> &stack) {
        Option<int> count = stack.pop_back();
        if (!count.hasValue()) {
            return;
        }

        int countValue = *count.getValue();
        int currentMin = INT32_MAX;

        for (int i = 0; i < countValue; i++) {
            Option<int> numOption = stack.pop_back();
            if (!numOption.hasValue()) {
                return;
            }
            int num = *numOption.getValue();
            if (currentMin > num) {
                currentMin = num;
            }
        }

        stack.push_back(new int(currentMin));
    }
}

class Token {
public:
    enum TYPE {
        ADD,
        SUB,
        MUL,
        DIV,
        NEG,
        IF,
        MAX,
        MIN,
        NUMBER,
        LEFT_BRACKET,
        NONE
    } operation;
    int priority;
    int number; // if NUMBER then NUMBER; if MAX or MIN then number of arguments

    explicit Token(char c) {
//        type = TYPE::TYPE;
        number = 0;
        switch (c) {
            case '+':
                operation = TYPE::ADD;
                priority = 1;
                break;
            case '-':
                operation = TYPE::SUB;
                priority = 1;
                break;
            case '*':
                operation = TYPE::MUL;
                priority = 2;
                break;
            case '/':
                operation = TYPE::DIV;
                priority = 2;
                break;
            case 'N':
                operation = TYPE::NEG;
                priority = 3;
                break;
            case 'F':
                operation = TYPE::IF;
                priority = 3;
                break;
            case 'I':
                operation = TYPE::MIN;
                priority = 4;
                break;
            case 'A':
                operation = TYPE::MAX;
                priority = 4;
                break;
            case '(':
                operation = TYPE::LEFT_BRACKET;
                priority = 0;
                break;
            default:
                throw std::invalid_argument("Invalid operation");
        }
    }

    explicit Token(int num) {
        operation = TYPE::NUMBER;
        number = num;
        priority = 99;
    }

    Token(char c, int num) {
        number = num;
        priority = 4;
        switch (c) {
            case 'I':
                operation = TYPE::MIN;
                break;
            case 'A':
                operation = TYPE::MAX;
                break;
            default:
                throw std::invalid_argument("Invalid operation");
        }
    }

    Token() {
        operation = TYPE::NONE;
        priority = 0;
        number = 0;
    }

    static char toChar(enum TYPE operation) {
        switch (operation) {
            case ADD:
                return '+';
            case SUB:
                return '-';
            case MUL:
                return '*';
            case DIV:
                return '/';
            case NEG:
                return 'N';
            case IF:
                return 'F';
            case MAX:
                return 'A';
            case MIN:
                return 'I';
            case NONE:
            default:
                return ' ';
        }
    }
};

void flushStackUntil(int priority, Stack<Token> &source, Stack<Token> &sink) {
    if (!source.peek_back().hasValue()) {
        return;
    }
    const Token *tokenPtr = source.peek_back().getValue();
    Token token;

    while (tokenPtr->priority >= priority) {
        token = *source.pop_back().getValue();
        if (token.operation == Token::TYPE::LEFT_BRACKET) {
            break;
        }
        sink.push_back(new Token(token));
        if (!source.peek_back().hasValue()) {
            return;
        }
        tokenPtr = source.peek_back().getValue();
    }
}

class Parser {
    bool dotFound;
    Stack<Token> stack;
    Stack<Token> postFixEquation;
public:
    Parser() : dotFound(false) {}

    /**
     * @brief Reset parser state and parse input from stdin
     * @return Postfix equation
     */
    Stack<Token> operator()() {
        dotFound = false;
        stack.clear();
        postFixEquation.clear();
        return parse();
    }

    Stack<Token> parse() {
        int count = 0;
        recursiveParseSingleLine(count, true);
        return postFixEquation;
    }

    void recursiveParseSingleLine(int &count, bool isMainLoop) {
        char buff[12];
        int bracketCount = 0;
        //scanf("%s", buff);
        std::cin >> buff;
        while (!dotFound) {
            if (buff[0] == '.') {
                dotFound = true;
                break;

            } else if (buff[0] == ',') {
                count++;

            } else if ('0' <= buff[0] && buff[0] <= '9') {
                int num = atoi(buff);
                postFixEquation.push_back(new Token(num));

                if (stack.peek_back().hasValue()
                    && stack.peek_back().getValue()->operation == Token::TYPE::NEG) {
                    Token token = *stack.pop_back().getValue();
                    postFixEquation.push_back(new Token(token));
                }

            } else if (buff[0] == '(') {
                bracketCount++;
                stack.push_back(new Token(buff[0]));

            } else if (buff[0] == '+' || buff[0] == '-') {
                stack.push_back(new Token(buff[0]));

            } else if (buff[0] == '*' || buff[0] == '/') {
                flushStackUntil(charToPriority(buff[0]), stack, postFixEquation);
                stack.push_back(new Token(buff[0]));

            } else if (buff[0] == 'N') {
                // negation
                Option<Token> peek = stack.peek_back();
                if (peek.hasValue() && peek.getValue()->operation == Token::TYPE::NEG) {
                    stack.push_back(new Token('N'));
                } else {
                    flushStackUntil(charToPriority('N'), stack, postFixEquation);
                    stack.push_back(new Token('N'));
                }

            } else if (buff[0] == 'I') {
                // if
                flushStackUntil(charToPriority('F'), stack, postFixEquation);
                recursiveParseSingleLine(count, false);
                postFixEquation.push_back(new Token('F'));

            } else if (buff[1] == 'I' || buff[1] == 'A') {
                // min or max
                int newCount = 1;
                flushStackUntil(charToPriority(buff[1]), stack, postFixEquation);
                // maybe change from recursion to a loop with context array
                recursiveParseSingleLine(newCount, false);
                postFixEquation.push_back(new Token(buff[1], newCount));

            } else if (buff[0] == ')') {
                bracketCount--;
                flushStackUntil(charToPriority('('), stack, postFixEquation);
                if (bracketCount == 0 && !isMainLoop) {
                    return;
                }

            }
            //scanf("%s", buff);
            std::cin >> buff;
        }

        flushStackUntil(-1, stack, postFixEquation);
    }

    static int charToPriority(char c) {
        // TODO maybe move this to Token class as it seems more fitting
        switch (c) {
            case '+':
            case '-':
                return 1;
            case '*':
            case '/':
                return 2;
            case 'N':
            case 'F':
                return 3;
            case 'I':
            case 'A':
                return 4;
            case '(':
            case ')':
                return 0;
            default:
                throw std::invalid_argument("Invalid operation");
        }
    }
} parseStdInToPostfix;

class Equation {
    Stack<Token> postFixEquation;
    Stack<int> stack;

public:
    explicit Equation(Stack<Token> postFixEquation) : postFixEquation(std::move(postFixEquation)) {}

    explicit Equation() : postFixEquation() {}

    void evaluateAndPrint() {
        while (!postFixEquation.isEmpty()) {
            Token t = *postFixEquation.pop_back().getValue();

            if (t.operation != Token::TYPE::NUMBER) {

            }

            evaluateSingleOperation(t);
        }
    }

    void printOperationAndStack(const Token &t) {

    }

    void evaluateSingleOperation(const Token &t) {
        switch (t.operation) {
            case Token::TYPE::ADD:
                Operations::add(stack);
                break;
            case Token::TYPE::SUB:
                Operations::sub(stack);
                break;
            case Token::TYPE::MUL:
                Operations::mul(stack);
                break;
            case Token::TYPE::DIV:
                Operations::div(stack);
                break;
            case Token::TYPE::NEG:
                Operations::neg(stack);
                break;
            case Token::TYPE::IF:
                Operations::ifFunc(stack);
                break;
            case Token::TYPE::MAX:
                stack.push_back(new int(t.number));
                Operations::max(stack);
                break;
            case Token::TYPE::MIN:
                stack.push_back(new int(t.number));
                Operations::min(stack);
                break;
            case Token::TYPE::NUMBER:
                stack.push_back(new int(t.number));
                break;
            default:
                break;
        }
    }
};

int main() {
//    freopen("test0.txt", "r", stdin); // input redirected in CLion compile settings
    Stack<Token> postFixEquation = parseStdInToPostfix();
    while (postFixEquation.peek_front().hasValue()) {
        Token token = *postFixEquation.pop_front().getValue();
        if (token.operation == Token::TYPE::NUMBER) {
            std::cout << token.number;
        } else if (token.operation == Token::TYPE::MAX || token.operation == Token::TYPE::MIN) {
            std::cout << ((Token::toChar(token.operation) == 'A') ? "MAX" : "MIN");
            std::cout << token.number;
        } else if (token.operation == Token::TYPE::IF) {
            std::cout << "IF";
        } else if (token.operation != Token::TYPE::NONE) {
            std::cout << Token::toChar(token.operation);
        }
        std::cout << ' ';
    }
    return 0;
}
