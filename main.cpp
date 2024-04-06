#include <iostream>

template<typename T>
class Option {
    T *_value;
    bool _hasValue;
    bool _isValueOwner;

    inline void deleteValueSafely() {
        if (_isValueOwner && _hasValue) {
            delete _value;
        }

        _value = nullptr;
    }

public:
    Option() : _value(nullptr), _hasValue(false), _isValueOwner(false) {}

    Option(const Option &other) {
        _value = new T(*other._value);
        _hasValue = other._hasValue;
        _isValueOwner = true;
    }

    Option(Option &&other) noexcept {
        _value = other._value;
        _hasValue = other._hasValue;
        _isValueOwner = other._isValueOwner;

        other._value = nullptr;
        other._hasValue = false;
        other._isValueOwner = false;
    }

    Option &operator=(const Option &other) {
        if (this == &other) {
            return *this;
        }
        deleteValueSafely();

        _value = new T(*other._value);
        _hasValue = other._hasValue;
        _isValueOwner = true;

        return *this;
    }

    Option &operator=(Option &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        deleteValueSafely();

        _value = other._value;
        _hasValue = other._hasValue;
        _isValueOwner = other._isValueOwner;

        other._value = nullptr;
        other._hasValue = false;
        other._isValueOwner = false;
        return *this;
    }

    explicit Option(T *value) : _value(value), _hasValue(true), _isValueOwner(false) {}

    explicit Option(T value) : _value(new T(value)), _hasValue(true), _isValueOwner(true) {}

    void setValue(T *value) {
        deleteValueSafely();
        _value = value;
        _hasValue = true;
        _isValueOwner = false;
    }

    void setValue(T value) {
        deleteValueSafely();
        _value = new T(value);
        _hasValue = true;
        _isValueOwner = true;
    }

    bool hasValue() const {
        return _hasValue;
    }

    const T *getValue() const {
        if (_hasValue) {
            return _value;
        }
        return nullptr;
    }

    ~Option() {
        deleteValueSafely();
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
            delete data;

        }
    };

    Node *head;
    Node *tail;

public:

    Stack() : head(nullptr), tail(nullptr) {}

    Stack(Stack &&other) noexcept: Stack() {
        head = other.head;
        tail = other.tail;

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

    Stack &operator=(Stack &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        clear();

        head = other.head;
        tail = other.tail;

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

    bool isEmpty() const {
        return head == nullptr && tail == nullptr;
    }

    /**
     * @brief Deletes all elements in the stack
     */
    void clear() {
//        while (head != nullptr) {
//            pop_back();
//        }
        Node *current = head;
        while (current != nullptr) {
            Node *next = current->next;
            delete current;
            current = next;
        }

        head = nullptr;
        tail = nullptr;
    }

    ~Stack() {
        clear();
    }

    void print();
};

template<>
void Stack<int>::print() {
    Node *current = tail;
    while (current != nullptr) {
        printf("%d ", *current->data);
        current = current->previous;
    }
    printf("\n");
}


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
            stack.push_back(new int(*val2.getValue() - *val1.getValue()));
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
            stack.push_back(new int(*val2.getValue() / *val1.getValue()));
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
            stack.push_back(new int(*val3.getValue() > 0 ? *val2.getValue() : *val1.getValue()));
        }
    }

    /**
     *  Needs argument count to be push_back'ed to the stack before calling
     */
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

    /**
     *  Needs argument count to be push_back'ed to the stack before calling
     */
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
    int number; // if NUMBER then just the number; if MAX or MIN then number of arguments

    explicit Token(char c) {
        number = 0;
        priority = charToPriority(c);

        switch (c) {
            case '(':
                operation = TYPE::LEFT_BRACKET;
                break;
            case '+':
                operation = TYPE::ADD;
                break;
            case '-':
                operation = TYPE::SUB;
                break;
            case '*':
                operation = TYPE::MUL;
                break;
            case '/':
                operation = TYPE::DIV;
                break;
            case 'N':
                operation = TYPE::NEG;
                break;
            case 'F':
                operation = TYPE::IF;
                break;
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

    static int charToPriority(char c) {
        switch (c) {
            case '(':
            case ')':
                return 0;
            case ',':
                // Unused in Token, but needed for Parser::recursiveParseSingleLine
                return 1;
            case '+':
            case '-':
                return 2;
            case '*':
            case '/':
                return 3;
            case 'N': // negation
                return 4;
            case 'F': // if
                return 5;
            case 'I': // min
            case 'A': // max
                return 6;
            default:
                throw std::invalid_argument("Invalid operation");
        }
    }

    void print() const {
        switch (operation) {
            case ADD:
                printf("+ ");
                break;
            case SUB:
                printf("- ");
                break;
            case MUL:
                printf("* ");
                break;
            case DIV:
                printf("/ ");
                break;
            case NEG:
                printf("N ");
                break;
            case IF:
                printf("IF ");
                break;
            case MAX:
                printf("MAX%d ", number);
                break;
            case MIN:
                printf("MIN%d ", number);
                break;
            case NUMBER:
                printf("%d ", number);
                break;
            default:
                break;
        }
    }
};

template<>
void Stack<Token>::print() {
    Node *current = head;
    while (current != nullptr) {
        current->data->print();
        printf(" "); // Adding a space to make it look like examples
        current = current->next;
    }
    printf("\n");
}

/**
 * @brief Move elements from one stack to the other until you reach a element with lower priority
 * @param priority the priority to stop at, if -1 then it will flush the whole stack
 */
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
    Stack<Token> &&operator()() {
        dotFound = false;
        stack.clear();
        postFixEquation.clear();
        return parse();
    }

    Stack<Token> &&parse() {
        int count = 0;
        recursiveParseSingleLine(count, true);
        return std::move(postFixEquation);
    }

    void recursiveParseSingleLine(int &argumentCount, bool isMainLoop) {
        char buff[12];
        int bracketCount = 0;
        scanf("%s", buff);

        while (!dotFound) {
            if (buff[0] == '.') {
                dotFound = true;
                break;

            } else if (buff[0] == ',') {
                argumentCount++;
                flushStackUntil(Token::charToPriority(buff[0]), stack, postFixEquation);

            } else if ('0' <= buff[0] && buff[0] <= '9') {
                int num = atoi(buff);
                postFixEquation.push_back(new Token(num));

                // push negation if it was the last encountered operation
                if (stack.peek_back().hasValue()
                    && stack.peek_back().getValue()->operation == Token::TYPE::NEG) {
                    Token token = *stack.pop_back().getValue();
                    postFixEquation.push_back(new Token(token));
                }

            } else if (buff[0] == '(') {
                bracketCount++;
                stack.push_back(new Token(buff[0]));

            } else if (buff[0] == '+' || buff[0] == '-') {
                flushStackUntil(Token::charToPriority(buff[0]), stack, postFixEquation);
                stack.push_back(new Token(buff[0]));
                
            } else if (buff[0] == '*' || buff[0] == '/') {
                flushStackUntil(Token::charToPriority(buff[0]), stack, postFixEquation);
                stack.push_back(new Token(buff[0]));

            } else if (buff[0] == 'N') {
                // negation
                Option<Token> peek = stack.peek_back();
                if (peek.hasValue() && peek.getValue()->operation == Token::TYPE::NEG) {
                    stack.push_back(new Token('N'));
                } else {
                    flushStackUntil(Token::charToPriority('N'), stack, postFixEquation);
                    stack.push_back(new Token('N'));
                }

            } else if (buff[0] == 'I') {
                // if
                int newCount = 0;
                flushStackUntil(Token::charToPriority('F'), stack, postFixEquation);
                recursiveParseSingleLine(newCount, false);
                postFixEquation.push_back(new Token('F'));

            } else if (buff[1] == 'I' || buff[1] == 'A') {
                // min or max
                int newCount = 1;
                flushStackUntil(Token::charToPriority(buff[1]), stack, postFixEquation);
                // maybe change from recursion to a loop with context array
                recursiveParseSingleLine(newCount, false);
                postFixEquation.push_back(new Token(buff[1], newCount));

            } else if (buff[0] == ')') {
                bracketCount--;
                flushStackUntil(Token::charToPriority('('), stack, postFixEquation);
                if (bracketCount == 0 && !isMainLoop) {
                    return;
                }

            }
            scanf("%s", buff);
        }

        flushStackUntil(-1, stack, postFixEquation);
    }

} parseStdInToPostfix;

class Equation {
    Stack<Token> postFixEquation;
    Stack<int> stack;

public:
    explicit Equation(Stack<Token> &&postFixEquation) : postFixEquation(std::move(postFixEquation)) {}

    explicit Equation() : postFixEquation() {}

    void evaluateAndPrint() {
        while (!postFixEquation.isEmpty()) {
            Token t = *postFixEquation.pop_front().getValue();
            if (t.operation != Token::TYPE::NUMBER) {
                t.print();
                stack.print();
            }
            if (t.operation == Token::TYPE::DIV && *stack.peek_back().getValue() == 0) {
                printf("ERROR\n");
                return;
            }
            evaluateSingleOperation(t);
        }

        Option<int> answer = stack.pop_back();
        if (answer.hasValue()) {
            printf("%d\n", *answer.getValue());
        }
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
    int equationCount;
    scanf("%d", &equationCount);

    for (int i = 0; i < equationCount; i++) {
        Stack<Token> postFixEquation = parseStdInToPostfix();
        postFixEquation.print();
        Equation equation(std::move(postFixEquation));
        equation.evaluateAndPrint();
        printf("\n");
    }
    return 0;
}
