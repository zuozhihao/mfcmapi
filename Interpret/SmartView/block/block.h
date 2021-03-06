#pragma once

namespace smartview
{
	class block
	{
	public:
		block() = default;

		const std::wstring& getText() const { return text; }
		const std::vector<block>& getChildren() const { return children; }
		bool isHeader() const { return cb == 0 && offset == 0; }

		virtual std::wstring ToString() const
		{
			std::vector<std::wstring> items;
			items.reserve(children.size() + 1);
			items.push_back(blank ? L"\r\n" : text);

			for (const auto& item : children)
			{
				items.push_back(item.ToString());
			}

			return strings::join(items, strings::emptystring);
		}

		size_t getSize() const { return cb; }
		void setSize(size_t _size) { cb = _size; }
		size_t getOffset() const { return offset; }
		void setOffset(size_t _offset) { offset = _offset; }
		ULONG getSource() const { return source; }
		void setSource(ULONG _source)
		{
			source = _source;
			for (auto& child : children)
			{
				child.setSource(_source);
			}
		}
		template <typename... Args> void addHeader(const std::wstring& _text, Args... args)
		{
			children.emplace_back(block(strings::formatmessage(_text.c_str(), args...)));
		}

		template <typename... Args> void setText(const std::wstring& _text, Args... args)
		{
			text = strings::formatmessage(_text.c_str(), args...);
		}

		void addBlock(const block& child, const std::wstring& _text)
		{
			auto block = child;
			block.text = _text;
			children.push_back(block);
		}

		template <typename... Args> void addBlock(const block& child, const std::wstring& _text, Args... args)
		{
			auto block = child;
			block.text = strings::formatmessage(_text.c_str(), args...);
			children.push_back(block);
		}

		// Add a block as a child
		void addBlock(const block& child)
		{
			auto block = child;
			block.text = child.ToStringInternal();
			children.push_back(block);
		}

		// Copy a block into this block with text
		template <typename... Args> void setBlock(const block& _data, const std::wstring& _text, Args... args)
		{
			text = strings::formatmessage(_text.c_str(), args...);
			children = _data.children;
		}

		void terminateBlock()
		{
			if (children.size() >= 1)
			{
				children.back().terminateBlock();
			}
			else
			{
				text = strings::ensureCRLF(text);
			}
		}

		void addBlankLine()
		{
			auto child = block();
			child.blank = true;
			children.emplace_back(child);
		}

		bool hasData() const { return !text.empty() || !children.empty(); }

	protected:
		size_t offset{};
		size_t cb{};
		ULONG source{};

	private:
		explicit block(std::wstring _text) : offset(0), cb(0), text(std::move(_text)) {}
		virtual std::wstring ToStringInternal() const { return text; }
		std::wstring text;
		std::vector<block> children;
		bool blank{false};
	};
} // namespace smartview