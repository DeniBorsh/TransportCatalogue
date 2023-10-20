#include "json_builder.h"
#include <iostream>

using namespace std;

namespace json {

	Builder::Builder() {
		nodes_stack_.emplace_back(&root_);
	}

	KeyContext<Builder> BuilderContext::Key(string key) {
		key_ = std::move(key);
		return *this;
	}

	Builder& Builder::Value(Node::Value value) {
		Node* top_node = nodes_stack_.back();

		if (top_node->IsDict()) {
			Dict& dict = get<Dict>(top_node->GetValue());
			auto [pos, _] = dict.emplace(move(key_.value()), GetNode(move(value)));
			key_ = nullopt;
		}
		else if (top_node->IsArray()) {
			Array& array = get<Array>(top_node->GetValue());
			array.emplace_back(GetNode(move(value)));
		}
		else if (root_.IsNull()) {
			root_.GetValue() = move(move(value));
		}

		return *this;
	}


	DictContext<Builder> Builder::StartDict() {
		Node* top_node = nodes_stack_.back();

		if (top_node->IsDict()) {
			Dict& dict = get<Dict>(top_node->GetValue());
			auto [pos, _] = dict.emplace(move(key_.value()), Dict());
			key_ = nullopt;
			nodes_stack_.emplace_back(&pos->second);
		}
		else if (top_node->IsArray()) {
			Array& array = get<Array>(top_node->GetValue());
			array.emplace_back(Dict());
			nodes_stack_.emplace_back(&array.back());
		}
		else if (root_.IsNull()) {
			top_node->GetValue() = Dict();
		}

		return *this;
	}

	ArrayContext<Builder> Builder::StartArray() {
		Node* top_node = nodes_stack_.back();

		if (top_node->IsDict()) {
			Dict& dict = get<Dict>(top_node->GetValue());
			auto [pos, _] = dict.emplace(move(key_.value()), Array());
			key_ = nullopt;
			nodes_stack_.emplace_back(&pos->second);
		}
		else if (top_node->IsArray()) {
			Array& array = get<Array>(top_node->GetValue());
			array.emplace_back(Array());
			nodes_stack_.emplace_back(&array.back());
		}
		else if (root_.IsNull()) {
			top_node->GetValue() = Array();
		}

		return *this;
	}

	Builder& BuilderContext::EndDict() {
		nodes_stack_.pop_back();
		return *this;
	}

	Builder& BuilderContext::EndArray() {
		nodes_stack_.pop_back();
		return *this;
	}

	Node Builder::Build() const {
		if (root_.IsNull() || nodes_stack_.size() > 1) throw logic_error("Не все операции завершены");

		return root_;
	}

	Node Builder::GetNode(Node::Value value) const {
		if (holds_alternative<int>(value)) return Node(get<int>(value));
		if (holds_alternative<double>(value)) return Node(get<double>(value));
		if (holds_alternative<string>(value)) return Node(get<string>(value));
		if (holds_alternative<nullptr_t>(value)) return Node(get<nullptr_t>(value));
		if (holds_alternative<bool>(value)) return Node(get<bool>(value));
		if (holds_alternative<Dict>(value)) return Node(get<Dict>(value));
		if (holds_alternative<Array>(value)) return Node(get<Array>(value));
		return {};
	}
}

