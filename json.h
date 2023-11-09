#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;
    using Dict = std::map<std::string, Node>;// Сохраните объявления Dict и Array без изменения
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {// Эта ошибка должна выбрасываться при ошибках парсинга JSON
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
        Node();
        Node(bool val);
        Node(double val);
        Node(int val);
        Node(std::string val);
        Node(nullptr_t val);
        Node(Array val);
        Node(Dict val);
        //--------------IS----------------------

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;
        //-------------AS----------------------
        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        double AsDouble() const;
        bool AsBool() const;
        const std::string& AsString() const;
        const Value& GetValue() const;

    private:
        Value value_;
    };


    /*Объекты Node можно сравнивать между собой при помощи == и !=. Значения равны, если внутри них значения имеют одинаковый тип и содержимое.*/
    inline bool operator==(const Node& rhs, const Node& lhs) {
        if (rhs.GetValue() == lhs.GetValue()) {
            return true;
        }
        return false;
    }
    inline bool operator!=(const Node& rhs, const Node& lhs) {
        return !(rhs == lhs);
    }


    class Document {
    public:
        explicit Document(Node root);
        const Node& GetRoot() const;
    private:
        Node root_;
    };


    inline bool operator==(const Document& rhs, const Document& lhs) {
        return rhs.GetRoot() == lhs.GetRoot();
    }
    inline bool operator !=(const Document& rhs, const Document& lhs) {
        return !(rhs == lhs);
    }

    //namespace Loader {
    Document Load(std::istream& input);
    Node LoadNode(std::istream& input);
    Node LoadNumber(std::istream& input);
    Node LoadString(std::istream& input);
    Node LoadArray(std::istream& input);
    Node LoadDict(std::istream& input);
    //}//namespace loader

    //namespace Printer {
    void Print(const Document& doc, std::ostream& output);
    struct PrintContext {// Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;
        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }
        PrintContext Indented() const {// Возвращает новый контекст вывода с увеличенным смещением
            return { out, indent_step, indent_step + indent };
        }
    };
    void PrintNode(const Node& node, const PrintContext& ctx);//напечатать ноду
    void PrintValue(std::nullptr_t, const PrintContext& ctx);//напечатать нулевой указатель
    void PrintValue(const bool val, const PrintContext& ctx);//напечатать булевое значение
    template <typename Val>
    void PrintValue(const Val& value, const PrintContext& ctx) {//напечатать int или double
        ctx.out << value;
    }
    void PrintValue(const std::string& value, const PrintContext& ctx);//напечатать строку
    void PrintValue(const Array& arr, const PrintContext& ctx);//напечатать массив
    void PrintValue(const Dict& map_, const PrintContext& ctx);//напечатать мапу

    //}//namespace printer



}  // namespace json