#pragma once
#include "json.h"

#include <optional>

namespace json {
	template <typename Parent> class DictContext;
	template <typename Parent> class KeyContext;
	template <typename Parent> class ArrayContext;

	class Builder {
	public:
		Builder();
		Builder& Value(json::Node::Value value);
		DictContext<Builder> StartDict();
		ArrayContext<Builder> StartArray();
		//KeyContext<Builder> Key(std::string key);
		//Builder& EndDict();
		//Builder& EndArray();
		json::Node Build() const;
		Builder& GetReference() { return *this; }

	protected:
		Node root_{ nullptr };
		std::vector<Node*> nodes_stack_;
		std::optional<std::string> key_{ std::nullopt };
		Node GetNode(Node::Value value) const;
	};

	class BuilderContext : public Builder {
	public:
		KeyContext<Builder> Key(std::string key);
		Builder& EndDict();
		Builder& EndArray();
	};

	template <typename Parent>
	class KeyContext {
	public:
		KeyContext(Builder& builder) : builder_(static_cast<BuilderContext*>(&builder)) {}

		Parent Value(json::Node::Value value) {
			return builder_->Value(value);
		}
		DictContext<Parent> StartDict() {
			return builder_->StartDict().GetReference();
		}
		ArrayContext<Parent> StartArray() {
			return builder_->StartArray().GetReference();
		}
		Builder& GetReference() {
			return *builder_;
		}

	private:
		BuilderContext* builder_ = nullptr;
	};

	template <typename Parent>
	class DictContext {
	public:
		DictContext(Builder& builder) : builder_(static_cast<BuilderContext*>(&builder)) {}

		KeyContext<DictContext<Parent>> Key(std::string key) {
			return builder_->Key(key).GetReference();
		}
		Parent EndDict() {
			return builder_->EndDict().GetReference();
		}
		Builder& GetReference() {
			return *builder_;
		}

	private:
		BuilderContext* builder_ = nullptr;
	};

	template <typename Parent>
	class ArrayContext {
	public:
		ArrayContext(Builder& builder) : builder_(static_cast<BuilderContext*>(&builder)) {}

		Parent EndArray() {
			return builder_->EndArray().GetReference();
		}
		ArrayContext<Parent> Value(json::Node::Value value) {
			ArrayContext<Parent> tmp = builder_->Value(value);
			return std::move(tmp);
		}
		DictContext<ArrayContext<Parent>> StartDict() {
			return builder_->StartDict().GetReference();
		}
		ArrayContext<ArrayContext<Parent>> StartArray() {
			return builder_->StartArray().GetReference();
		}
		Builder& GetReference() {
			return *builder_;
		}

	private:
		BuilderContext* builder_ = nullptr;
	};
}

