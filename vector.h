#pragma once
#include <cstddef>
#include <new>
#include <utility>
#include <initializer_list>
#include <memory>

namespace mySTL
{
	template<typename T>
	class vector
	{
	public:
		using iterator = T*;
		using size_type = std::size_t;
		using const_iterator = const T*;
		using alloc_traits = std::allocator_traits<std::allocator<T>>;
	private:
		static constexpr size_type default_memory = 200;
		iterator begin_;
		iterator end_;
		iterator end_of_storage_;
		std::allocator<T> alloc_;
	public:
		// 构造函数
		vector() noexcept :begin_(nullptr), end_(nullptr), end_of_storage_(nullptr) {}

		vector(size_type n) :begin_(nullptr), end_(nullptr), end_of_storage_(nullptr)
		{
			if (n == 0) return;

			size_type newCap = ((n + default_memory - 1) / default_memory) * default_memory;
			begin_ = alloc_.allocate(newCap);

			end_ = begin_;
			try {
				for (size_type i = 0; i < n; ++i) {
					alloc_traits::construct(alloc_, end_);
					++end_;
				}
				end_of_storage_ = begin_ + newCap;
			}
			catch (...) {
				while (end_ != begin_) {
					--end_;
					alloc_traits::destroy(alloc_, end_);
				}
				alloc_.deallocate(begin_, newCap);
				begin_ = end_ = end_of_storage_ = nullptr;
				throw;
			}
		}

		vector(size_type n, const T& value) :begin_(nullptr), end_(nullptr), end_of_storage_(nullptr)
		{
			if (n == 0) return;

			size_type newCap = ((n + default_memory - 1) / default_memory) * default_memory;
			begin_ = alloc_.allocate(newCap);

			end_ = begin_;
			try {
				for (size_type i = 0; i < n; ++i) {
					alloc_traits::construct(alloc_, end_, value);
					++end_;
				}
				end_of_storage_ = begin_ + newCap;
			}
			catch (...) {
				while (end_ != begin_) {
					--end_;
					alloc_traits::destroy(alloc_, end_);
				}
				alloc_.deallocate(begin_, newCap);
				begin_ = end_ = end_of_storage_ = nullptr;
				throw;
			}
		}

		vector(std::initializer_list<T> il) :begin_(nullptr), end_(nullptr), end_of_storage_(nullptr)
		{
			size_type n = il.size();
			if (n == 0) return;

			size_type newCap = ((n + default_memory - 1) / default_memory) * default_memory;
			begin_ = alloc_.allocate(newCap);

			end_ = begin_;
			try {
				for (const T& elem : il)
				{
					alloc_traits::construct(alloc_, end_, elem);
					++end_;
				}
				end_of_storage_ = begin_ + newCap;
			}
			catch (...) {
				while (end_ != begin_) {
					--end_;
					alloc_traits::destroy(alloc_, end_);
				}
				alloc_.deallocate(begin_, newCap);
				begin_ = end_ = end_of_storage_ = nullptr;
				throw;
			}
		}

		vector(const vector<T>& other) :begin_(nullptr), end_(nullptr), end_of_storage_(nullptr)
		{
			size_type n = other.size();
			size_type cap = other.capacity();
			if (n == 0) return;

			begin_ = alloc_.allocate(cap);

			end_ = begin_;
			try {
				for (const T& elem : other)
				{
					alloc_traits::construct(alloc_, end_, elem);
					++end_;
				}
				end_of_storage_ = begin_ + cap;
			}
			catch (...) {
				while (end_ != begin_) {
					--end_;
					alloc_traits::destroy(alloc_, end_);
				}
				alloc_.deallocate(begin_, cap);
				begin_ = end_ = end_of_storage_ = nullptr;
				throw;
			}
		}

		vector(vector&& other) noexcept : begin_(other.begin_),
			end_(other.end_), end_of_storage_(other.end_of_storage_), alloc_(std::move(other.alloc_))
		{
			other.begin_ = other.end_ = other.end_of_storage_ = nullptr;
		}

		vector& operator=(const vector<T>& other)
		{
			if (this == &other) return *this;
			if (begin_ != nullptr) {
				while (end_ != begin_) {
					--end_;
					alloc_traits::destroy(alloc_, end_);
				}
				alloc_.deallocate(begin_, capacity());
				begin_ = end_ = end_of_storage_ = nullptr;
			}

			size_type n = other.size();
			size_type cap = other.capacity();
			if (n == 0) return *this;

			begin_ = alloc_.allocate(cap);

			end_ = begin_;
			try {
				for (const T& elem : other)
				{
					alloc_traits::construct(alloc_, end_, elem);
					++end_;
				}
				end_of_storage_ = begin_ + cap;
			}
			catch (...) {
				while (end_ != begin_) {
					--end_;
					alloc_traits::destroy(alloc_, end_);
				}
				alloc_.deallocate(begin_, cap);
				begin_ = end_ = end_of_storage_ = nullptr;
				throw;
			}
			return *this;
		}

		vector& operator=(vector<T>&& other) noexcept
		{
			if (this == &other) return *this;

			if (begin_ != nullptr) {
				while (end_ != begin_) {
					--end_;
					alloc_traits::destroy(alloc_, end_);
				}
				alloc_.deallocate(begin_, static_cast<size_type>(end_of_storage_ - begin_));
			}

			begin_ = other.begin_;
			end_ = other.end_;
			end_of_storage_ = other.end_of_storage_;
			alloc_ = std::move(other.alloc_);
			other.begin_ = other.end_ = other.end_of_storage_ = nullptr;
			return *this;
		}

		~vector()
		{
			if (begin_ == nullptr) return;
			while (end_ != begin_) {
				--end_;
				alloc_traits::destroy(alloc_, end_);
			}
			alloc_.deallocate(begin_, capacity());
		}

		// 容量管理函数
		size_type size() const noexcept
		{
			if (begin_ == nullptr) return 0;
			return end_ - begin_;
		}

		size_type capacity() const noexcept
		{
			if (begin_ == nullptr) return 0;
			return end_of_storage_ - begin_;
		}

		bool empty() const noexcept
		{
			return begin_ == end_;
		}

		void reserve(size_type n)
		{

			if (capacity() >= n) return;

			size_type old_cap = capacity();
			size_type newCap = ((n + default_memory - 1) / default_memory) * default_memory;

			iterator newBegin = alloc_.allocate(newCap);
			iterator newEnd = newBegin;

			try {
				iterator cur = begin_;
				while (cur != end_) {
					alloc_traits::construct(alloc_, newEnd, std::move(*cur));
					++cur;
					++newEnd;
				}
			}
			catch (...) {
				while (newEnd != newBegin) {
					--newEnd;
					alloc_traits::destroy(alloc_, newEnd);
				}
				alloc_.deallocate(newBegin, newCap);
				throw;
			}

			while (end_ != begin_) {
				--end_;
				alloc_traits::destroy(alloc_, end_);
			}
			if (begin_) {
				alloc_.deallocate(begin_, old_cap);
			}

			begin_ = newBegin;
			end_ = newEnd;
			end_of_storage_ = newBegin + newCap;
		}

		void resize(size_type n)
		{
			size_type oldSize = size();
			if (n > capacity())
			{
				reserve(n);
			}
			if (n > oldSize)
			{
				try {
					while (end_ - begin_ < n) {
						alloc_traits::construct(alloc_, end_);
						++end_;
					}
				}
				catch (...) {
					while (end_ - begin_ > oldSize) {
						--end_;
						alloc_traits::destroy(alloc_, end_);
					}
					throw;
				}
			}
			else
			{
				while (end_ - begin_ > n) {
					--end_;
					alloc_traits::destroy(alloc_, end_);
				}
			}
		}

		// 元素分配
		void push_back(const T& value)
		{
			if (end_ == end_of_storage_) 
			{
				size_type newCap = capacity() + default_memory;
				reserve(newCap);
			}
			alloc_traits::construct(alloc_, end_, value);
			++end_;
		}

		void push_back(T&& value)
		{
			if (end_ == end_of_storage_)
				reserve(capacity() == 0 ? default_memory : capacity() + default_memory);

			alloc_traits::construct(alloc_, end_, std::move(value));
			++end_;
		}


		void pop_back()
		{
			if (end_ == begin_) return;
			--end_;
			alloc_traits::destroy(alloc_, end_);
		}

		void insert(size_type pos, const T& value)
		{
			if (pos > size()) return;
			if (pos == size()) {
				push_back(value);
				return;
			}
			if (end_ == end_of_storage_) {
				size_type newCap = capacity() == 0 ? default_memory : capacity() + default_memory;
				reserve(newCap);
			}

			iterator insertPos = begin_ + pos;

			alloc_traits::construct(alloc_, end_, std::move(*(end_ - 1)));
			++end_;

			for (iterator cur = end_ - 2; cur != insertPos; --cur) {
				*cur = std::move(*(cur - 1));
			}

			*insertPos = value;
		}


		void erase(size_type pos)
		{
			if (pos >= size()) {
				return;
			}

			iterator first = begin_ + pos;
			iterator last = end_;

			for (iterator cur = first + 1; cur != last; ++cur) {
				*(cur - 1) = std::move(*cur);
			}
			--end_;
			alloc_traits::destroy(alloc_, end_);
		}

		// 迭代器
		iterator begin()
		{
			return begin_;
		}
		iterator end()
		{
			return end_;
		}
		const_iterator begin() const noexcept
		{
			return begin_;
		}
		const_iterator end() const noexcept
		{
			return end_;
		}
		T& front() { return *begin_; }
		T& back() { return *(end_ - 1); }
		const T& front() const { return *begin_; }
		const T& back()  const { return *(end_ - 1); }

		// 下标访问
		T& operator[](size_type i) 
		{ 
			return begin_[i]; 
		}
		const T& operator[](size_type i) const 
		{ 
			return begin_[i]; 
		}
	};
}
