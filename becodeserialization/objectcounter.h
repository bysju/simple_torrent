#ifndef _OBJECT_COUNTER_H_
#define _OBJECT_COUNTER_H_

template <typename T>
class ObjectCounter
{
public:
	static int object_created;
	static int object_alived;

	ObjectCounter()
	{
		++object_created;
		++object_alived;
	}
protected:
	// object should never be removed through pointers of the type
	~ObjectCounter()
	{
		--object_alived;
	}
};

template<typename T> int ObjectCounter<T>::object_created(0);
template<typename T> int ObjectCounter<T>::object_alived(0);


#endif //_OBJECT_COUNTER_H_