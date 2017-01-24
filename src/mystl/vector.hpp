#pragma once

#include <mystl/base/VectorIterator.hpp>
#include <mystl/base/ConstIterator.hpp>
#include <mystl/base/ReverseIterator.hpp>

#include <mystl/functional.hpp>

namespace mystl
{
	template <typename T>
	class vector
	{
	public:
		typedef base::VectorIterator<T> iterator;
		typedef base::ConstIterator<iterator> const_iterator;
		typedef base::ReverseIterator<iterator> reverse_iterator;
		typedef base::ConstIterator<reverse_iterator> const_reverse_iterator;

	private:
		/**
		 * @hidden
		 */
		T *data_;

		/**
		 * @hidden
		 */
		size_t size_;

		/**
		 * @hidden
		 */
		size_t capacity_;

	public:
		/* ==========================================================
			CONSTRUCTORS
				- BASIC CONSTRUCTORS
				- ASSIGNERS
		=============================================================
			BASIC CONSTRUCTOR
		---------------------------------------------------------- */
		/**
		 * Default Constructor.
		 */
		vector()
		{
			this->data_ = nullptr;
			this->size_ = 0;
			this->capacity_ = 0;
		};

		/* ----------------------------------------------------------
			ASSIGNERS
		---------------------------------------------------------- */
		void reserve(size_t n)
		{
			if (n < this->capacity_)
				return;

			this->_Set_capacity(n);
		};

		void resize(size_t n)
		{
			if (n <= this->size_)
			{
				this->size_ = n;
				return;
			}

			this->_Expand_capacity(n);
			this->size_ = n;
		};

		void resize(size_t n, const T &val)
		{
			size_t old_size = n > this->size_;
			this->resize(n);

			if (this->size_ > old_size)
			{
				T *last = this->data_ + this->size_;
				for (T *it = this->data_ + old_size; it != last; it++)
					*it = val;
			}
		};

		void shrink_to_fit()
		{
			if (this->capacity_ > this->size_)
				this->_Expand_capacity(this->size_);
		};

	private:
		void _Expand_capacity(size_t n)
		{
			// NEW ARRAY
			T *data = new T[n];

			if (this->size_ != 0)
			{
				// MIGRATE DATA
				T *last = this->data_ + this->size_;
				for (T *it = this->data_; it != last; it++)
					*(data++) = std::move(*it);
			}

			// ORDINARY DATA ARE TRUNCATED
			delete[] this->data_;

			// ASSIGN NEW MEMBERS
			this->data_ = data - this->size_;
			this->capacity_ = n;
		};

	public:
		/* ==========================================================
			ACCESSORS
				- BASIC ELEMENTS
				- ITERATORS
				- INDEX ACCESSORS
		=============================================================
			BASIC ELEMENTS
		---------------------------------------------------------- */
		auto data() const -> const T*
		{
			return this->data_;
		};
		auto size() const -> size_t
		{
			return this->size_;
		};
		auto capacity() const -> size_t
		{
			return this->capacity_;
		};

		auto empty() const -> bool
		{
			return this->size_ != 0;
		};

		auto front() -> T&
		{
			return this->operator[](0);
		};
		auto front() const -> const T&
		{
			return this->operator[](0);
		};

		auto back() -> T&
		{
			return this->operator[](this->size_ - 1);
		};
		auto back() const -> const T&
		{
			return this->operator[](this->size_ - 1);
		};
		
		/* ----------------------------------------------------------
			ITERATORS
		---------------------------------------------------------- */
		auto begin() -> iterator
		{
			return iterator(this->data_);
		};
		auto begin() const -> const_iterator
		{
			return const_iterator(this->begin());
		};
		auto cbegin() const -> const_iterator
		{
			return const_iterator(this->begin());
		};

		auto rbegin() -> reverse_iterator
		{
			return reverse_iterator(this->end());
		};
		auto rbegin() const -> const_reverse_iterator
		{
			return const_reverse_iterator(this->rbegin());
		};
		auto crbegin() const -> const_reverse_iterator
		{
			return const_reverse_iterator(this->rbegin());
		};

		auto end() -> iterator
		{
			return iterator(this->data_ + this->size_);
		};
		auto end() const -> const_iterator
		{
			return const_iterator(this->end());
		};
		auto cend() const -> const_iterator
		{
			return const_iterator(this->end());
		};

		auto rend() -> reverse_iterator
		{
			return reverse_iterator(this->begin());
		};
		auto rend() const -> const_reverse_iterator
		{
			return reverse_iterator(this->begin());
		};
		auto crend() const -> const_reverse_iterator
		{
			return const_reverse_iterator(this->rend());
		};

		/* ----------------------------------------------------------
			INDEX ACCESSORS
		---------------------------------------------------------- */
		auto operator[](size_t index) -> T&
		{
			return this->data_[index];
		};
		auto operator[](size_t index) const -> const T&
		{
			return this->data_[index];
		};

		auto at(size_t index) -> T&
		{
			return this->operator[](index);
		};
		auto at(size_t index) const -> const T&
		{
			return this->operator[](index);
		};

	public:
		/* ==========================================================
			ELEMENTS I/O
				- PUSH & POP
				- INSERT
				- ERASE
		=============================================================
			PUSH & POP
		---------------------------------------------------------- */
		void push_back(const T &val)
		{
			if (this->capacity_ == this->size_)
				this->_Expand_capacity(max<size_t>(this->capacity_ + 1, this->capacity_ * 1.5));
			
			this->data_[this->size_++] = val;
		};

		void push_back(T &&val)
		{
			if (this->capacity_ == this->size_)
				this->_Expand_capacity(max<size_t>(this->capacity_ + 1, this->capacity_ * 1.5));

			this->data_[this->size_++] = std::forward<T>(val);
		};

		void pop_back()
		{
			this->size_--;
		};

		/* ----------------------------------------------------------
			INSERT
		---------------------------------------------------------- */
		auto insert(iterator it, const T &val) -> iterator
		{
			return iterator(it, (size_t)1, val);
		};

		auto insert(iterator it, T &&val) -> iterator
		{
			size_t index = (size_t)(it.operator->() - this->data_);
			if (index == this->size_)
			{
				size_t size = this->size_ + n;
				if (size > this->capacity_)
					this->_Expand_capacity(max<size_t>(size, this->capacity_ * 1.5));

				*(this->data_ + this->size_) = std::forward<T>(val);
				this->size_ = size;
			}
			else
			{
				size_t size = this->size_ + n;
				size_t capacity = this->_Expand_capacity(max<size_t>(size, this->capacity_ * 1.5));
				T *data = new T[capacity];

				T *x = this->data_;
				T *y = data;

				// DATA DATA IN THE FRONT
				for (; x != this->data_ + index; x++)
					*(y++) = *x;

				// MIDDLE, THE NEWLY INSERTED DATA
				*(y++) = std::forward<T>(val);

				// MIGRATE DATA IN THE BACK
				for (; x != this->data_ + this->size_; x++)
					*(y++) = *x;

				// NEWLY ASSIGNED MEMBERS
				this->data_ = data;
				this->size_ = size;
				this->capacity_ = capacity;
			}

			return iterator(this->data_ + index);
		};

		auto insert(iterator it, size_t n, const T &val) -> iterator
		{
			size_t index = (size_t)(it.operator->() - this->data_);
			if (index == this->size_)
			{
				size_t size = this->size_ + n;
				if (size > this->capacity_)
					this->_Expand_capacity(max<size_t>(size, this->capacity_ * 1.5));

				T *it = this->data_ + this->size_;
				while (n-- != 0)
					*(it++) = val;

				this->size_ = size;
			}
			else
			{
				size_t size = this->size_ + n;
				size_t capacity = this->_Expand_capacity(max<size_t>(size, this->capacity_ * 1.5));
				T *data = new T[capacity];

				T *x = this->data_;
				T *y = data;

				// DATA DATA IN THE FRONT
				for (; x != this->data_ + index; x++)
					*(y++) = *x;

				// MIDDLE, THE NEWLY INSERTED DATA
				for (size_t i = 0; i < n; i++)
					*(y++) = val;

				// MIGRATE DATA IN THE BACK
				for (; x != this->data_ + this->size_; x++)
					*(y++) = *x;

				// NEWLY ASSIGNED MEMBERS
				this->data_ = data;
				this->size_ = size;
				this->capacity_ = capacity;
			}

			return iterator(this->data_ + index);
		};

		template <class InputIterator>
		auto insert(iterator it, InputIterator first, InputIterator last) -> iterator
		{
			size_t index = (size_t)(it.operator->() - this->data_);
			if (index == this->size_)
			{
				size_t size = this->size_ + n;
				if (size > this->capacity_)
					this->_Expand_capacity(max<size_t>(size, this->capacity_ * 1.5));

				T *it = this->data_ + this->size_;
				for (; first != last; first++)
					*(it++) = *first;

				this->size_ = size;
			}
			else
			{
				size_t size = this->size_ + n;
				size_t capacity = this->_Expand_capacity(max<size_t>(size, this->capacity_ * 1.5));
				T *data = new T[capacity];

				T *x = this->data_;
				T *y = data;

				// DATA DATA IN THE FRONT
				for (; x != this->data_ + index; x++)
					*(y++) = *x;

				// MIDDLE, THE NEWLY INSERTED DATA
				for (size_t i = 0; i < n; i++)
					*(y++) = val;

				// MIGRATE DATA IN THE BACK
				for (; first != last; first++)
					*(y++) = *first;

				// NEWLY ASSIGNED MEMBERS
				this->data_ = data;
				this->size_ = size;
				this->capacity_ = capacity;
			}

			return iterator(this->data_ + index);
		};

		/* ----------------------------------------------------------
			ERASE
		---------------------------------------------------------- */
	};
};