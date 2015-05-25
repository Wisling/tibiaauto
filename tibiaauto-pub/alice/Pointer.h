/**
 * Pointer - A safer way to use pointers
 *
 * @author	Jonathan Roewen
 */
#ifndef POINTER_H
#define POINTER_H

using namespace std;    //	Otherwise this becomes a nightmare later if "Pointer.h" included in wrong order!

template <class T>
class Pointer
{
protected:
	class Holder
	{
public:
		T *datum;
		unsigned count;

		Holder(T *t) : datum(t), count(1)
		{
		}

		~Holder()
		{
			delete datum;
		}
	};

	Holder *value;

	//	I'm just curious .. how does this cope with NULL?
	void unbind()
	{
		if (--value->count == 0)
			delete value;
	}

	void bind(const Pointer<T> &rhs)
	{
		if (rhs.value != value)
		{
			unbind();
			value = rhs.value;
			value->count++;
		}
	}

	void bind(T *t)
	{
		if (value->datum != t)
		{
			unbind();
			value = new Holder(t);
		}
	}

public:
	explicit Pointer(T *t = NULL) : value(new Holder(t))
	{
	}

	Pointer(const Pointer<T> &rhs) : value(rhs.value)
	{
		value->count++;
	}

	~Pointer()
	{
		unbind();
	}

	Pointer<T> &operator=(const Pointer<T> &rhs)
	{
		//	Keep getting *bus error* for value->datum
		//	if (value->datum != NULL) {
		bind(rhs);
		//	} else {
		//		value = rhs.value;
		//	}
		return *this;
	}

	Pointer<T> &operator=(T *t)
	{
		//	if (t != NULL) {
		bind(t);
		//	} else {
		//		value = new Holder(t);
		//	}
		return *this;
	}

	T *operator->()
	{
		return value->datum;
	}

	T &operator*()
	{
		return *(value->datum);
	}

	const T *operator->() const
	{
		return value->datum;
	}

	const T &operator*() const
	{
		return *(value->datum);
	}

	bool operator==(const Pointer<T> &rhs) const
	{
		return (value->datum) == (rhs.value->datum);
	}

	bool operator!=(const Pointer<T> &rhs) const
	{
		return (value->datum) != (rhs.value->datum);
	}

	bool operator==(T *t) const
	{
		return (value->datum) == t;
	}

	bool operator!=(T *t) const
	{
		return (value->datum) != t;
	}
};

#endif // ifndef POINTER_H
