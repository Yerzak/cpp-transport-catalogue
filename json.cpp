#include "json.h"

using namespace std;

namespace json {

    //-------------------CLASS____NODE--------------------------------------
    //конструкторы класса NODE
    Node::Node() : value_(nullptr) {
    }
    Node::Node(bool val) : value_(val) {
    }
    Node::Node(double val) : value_(val) {
    }
    Node::Node(int val) : value_(val) {
    }
    Node::Node(std::string val) : value_(std::move(val)) {
    }
    Node::Node(nullptr_t val) : value_(val) {
    }
    Node::Node(Array val) : value_(std::move(val)) {
    }
    Node::Node(Dict val) : value_(std::move(val)) {
    }
    //---------------IS-------------------------

    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }
    bool Node::IsDouble() const {
        if (IsInt()) {
            return true;
        }
        return std::holds_alternative<double>(value_);
    }
    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }
    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }
    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }
    bool Node::IsNull() const {
        return std::holds_alternative<nullptr_t>(value_);
    }
    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }
    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(value_);
    }
    //----------------AS-------------------------


    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw std::logic_error("Not an array");
        }
        return std::get<Array>(value_);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) {
            throw std::logic_error("Not a map");
        }
        return std::get<Dict>(value_);
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("Not an integer");
        }
        return std::get<int>(value_);
    }

    double Node::AsDouble() const {
        if (!IsDouble()) {
            throw std::logic_error("Not a double");
        }
        if (IsInt()) {
            return static_cast<double>(std::get<int>(value_));
        }
        return static_cast<double>(std::get<double>(value_));
    }

    bool Node::AsBool() const {
        if (!IsBool()) {
            throw std::logic_error("Not a boolian");
        }
        return std::get<bool>(value_);
    }

    const std::string& Node::AsString() const {
        if (!IsString()) {
            throw std::logic_error("Not a string");
        }
        return std::get<std::string>(value_);
    }
    const Node::Value& Node::GetValue() const {
        return value_;
    }
    //-----------------CLASS NODE IS OVER------------------------------------------------
    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }
    //-------------------CLASS DOCUMENT IS OVER-----------------------------------------


    Node LoadNumber(std::istream& input) {
        using namespace std::literals;
        std::string parsed_num;
        auto read_char = [&parsed_num, &input] {// Считывает в parsed_num очередной символ из input
            parsed_num += static_cast<char>(input.get());
            if (!input) {
                throw ParsingError("Failed to read number from stream"s);
            }
            };
        auto read_digits = [&input, read_char] {// Считывает одну или более цифр в parsed_num из input
            if (!std::isdigit(input.peek())) {
                throw ParsingError("A digit is expected"s);
            }
            while (std::isdigit(input.peek())) {
                read_char();
            }
            };

        if (input.peek() == '-') {
            read_char();
        }
        if (input.peek() == '0') {// Парсим целую часть числа
            read_char();// После 0 в JSON не могут идти другие цифры            
        }
        else {
            read_digits();
        }

        bool is_int = true;// Парсим дробную часть числа        
        if (input.peek() == '.') {
            read_char();
            read_digits();
            is_int = false;
        }

        if (int ch = input.peek(); ch == 'e' || ch == 'E') {// Парсим экспоненциальную часть числа
            read_char();
            if (ch = input.peek(); ch == '+' || ch == '-') {
                read_char();
            }
            read_digits();
            is_int = false;
        }

        try {
            if (is_int) {// Сначала пробуем преобразовать строку в int                
                try {
                    return Node(std::stoi(parsed_num));
                }
                catch (...) {
                    // В случае неудачи, например, при переполнении,
                    // код ниже попробует преобразовать строку в double
                }
            }
            return Node(std::stod(parsed_num));
        }
        catch (...) {
            throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
        }
    }

    Node LoadString(std::istream& input) {// Считывает содержимое строкового литерала JSON-документа. 
        //Функцию следует использовать после считывания открывающего символа ":
        using namespace std::literals;
        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        std::string s;
        while (true) {
            if (it == end) {// Поток закончился до того, как встретили закрывающую кавычку?                
                throw ParsingError("String parsing error");
            }
            const char ch = *it;
            if (ch == '"') {// Встретили закрывающую кавычку                
                ++it;
                break;
            }
            else if (ch == '\\') {// Встретили начало escape-последовательности                
                ++it;
                if (it == end) {// Поток завершился сразу после символа обратной косой черты                    
                    throw ParsingError("String parsing error");
                }
                const char escaped_char = *(it);// Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"                
                switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:// Встретили неизвестную escape-последовательность                    
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                }
            }
            else if (ch == '\n' || ch == '\r') {// Строковый литерал внутри- JSON не может прерываться символами \r или \n                
                throw ParsingError("Unexpected end of line"s);
            }
            else {// Просто считываем очередной символ и помещаем его в результирующую строку                
                s.push_back(ch);
            }
            ++it;
        }
        return Node(s);
    }

    Node LoadArray(istream& input) {
        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        if (it == end) {
            throw ParsingError("Array input is empty");
        }
        Array result;
        /*if (input.peek() == input.eof()) {
            throw ParsingError("Array input is empty");
        }*/
        for (char c; input >> c && c != ']';) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }
        return Node(move(result));
    }

    Node LoadDict(istream& input) {
        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        if (it == end) {
            throw ParsingError("Array input is empty");
        }
        Dict result;
        bool IsFirst = true;// перед первым ключом не стоит запятая
        std::string key;
        for (char c; input >> c && c != '}';) {
            if (IsFirst) {//обработка первого ключа
                key = LoadString(input).AsString();//вызываем LoadNode, а не LoadString
                //чтобы отсеклись escape-последовательности и первые кавычки
                input >> c;
                IsFirst = false;
            }
            if (c == ',') {//обработка последующих ключей аналогично
                key = LoadNode(input).AsString();
                input >> c;
            }
            if (c == ':') {//обработка значений
                Node value = LoadNode(input);
                result.emplace(std::move(key), std::move(value));
            }
        }
        return Node(move(result));
    }

    Node LoadNode(istream& input) {
        using namespace std::literals;
        char c;
        do {
            input >> c;
        } while (c == '\n' || c == '\r' || c == '\t' || c == ' ' || c == '\\');
        if (c == '[') {//массив
            return LoadArray(input);
        }
        else if (c == '{') {//словарь
            return LoadDict(input);
        }
        else if (c == '"' || c == '\"') {//строка
            return LoadString(input);
        }
        else if (c == 't' || c == 'f') {//булевое значение
            std::string value;
            value += c;
            while (input >> c && (c == 't' || c == 'r' || c == 'u' || c == 'e' || c == 'f' || c == 'a' || c == 'l' || c == 's')) {
                value += c;
            }
            input.putback(c);
            if (value == "true"s) {
                return Node(move(true));
            }
            else if (value == "false") {
                return Node(move(false));
            }
            throw ParsingError("Your bool expression is unknown");
        }
        else if (c == 'n') {//нулл поинтер
            std::string value;
            value += c;
            while (input >> c && (c == 'n' || c == 'u' || c == 'l')) {
                value += c;
            }
            input.putback(c);
            if (value != "null"s) {
                throw ParsingError("your nullptr_t expression in unknown");
            }
            return Node(nullptr);
        }
        else if (std::isdigit(c) || c == '-') {//число
            input.putback(c);
            return LoadNumber(input);
        }
        else {
            throw ParsingError("Unknown data");
        }
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void PrintValue(std::nullptr_t, const PrintContext& ctx) {// Перегрузка функции PrintValue для вывода значений null
        ctx.out << "null"sv;
    }
    //Перегрузка функции для вывода булевых значений 
    void PrintValue(const bool val, const PrintContext& ctx) {
        if (val) {
            ctx.out << "true";
        }
        else {
            ctx.out << "false";
        }
    }

    void PrintValue(const std::string& value, const PrintContext& ctx) {
        ctx.out << '"';
        for (const char c : value) {
            switch (c) {
            case '\n':
                ctx.out << "\\n";
                break;
            case '\r':
                ctx.out << "\\r";
                break;
            case '\\':
                ctx.out << "\\\\";
                break;
            case '\"':
                ctx.out << '\\' << '"';
                break;
            default:
                ctx.out << c;
            }
        }
        ctx.out << '"';
    }

    void PrintValue(const Array& arr, const PrintContext& ctx) {//Перегрузка функции для вывода значений массива
        ctx.out << "[";
        bool IsFirst = true;
        for (const auto& elem : arr) {
            if (IsFirst) {
                PrintNode(elem, ctx);
                IsFirst = false;
            }
            else {
                ctx.out << ", ";
                PrintNode(elem, ctx);
            }
        }
        ctx.out << "]";
    }

    void PrintValue(const Dict& map_, const PrintContext& ctx) {//Перегрузка функции для вывода значений словаря
        ctx.out << "{";
        bool IsFirst = true;
        for (const auto& [key, value] : map_) {
            if (IsFirst) {
                ctx.out << "\"" << key << "\"" << " : ";
                PrintNode(value, ctx);
                IsFirst = false;
            }
            else {
                ctx.out << ", " << "\"" << key << "\"" << " : ";
                PrintNode(value, ctx);
            }
        }
        ctx.out << "}";
    }

    void PrintNode(const Node& node, const PrintContext& ctx) {
        std::visit(
            [&ctx](const auto& value) { PrintValue(value, ctx); },
            node.GetValue());
    }


    void Print(const Document& doc, std::ostream& output) {
        (void)&doc;
        (void)&output;
        PrintNode(doc.GetRoot(), PrintContext{ output });
    }

}  // namespace json