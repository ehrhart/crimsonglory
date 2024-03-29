#ifndef MMANAGER_H_INCLUDED
#define MMANAGER_H_INCLUDED

#include "engine.h"

class IMMObject
{
  private:
    IMMObject* nextObject;
    IMMObject* prevObject;
    long refCount;

    static IMMObject* liveObjects;
    static IMMObject* deadObjects;

  protected:
    IMMObject();
    virtual ~IMMObject();
  public:
    virtual void AddRef();
    virtual void Release();
    static void CollectGarbage();
    static void CollectRemainingObjects( bool bEmitWarnings = false );
    virtual unsigned long size() = 0;
    void* operator new( size_t size );
    void operator delete( void* obj );

    static std::list<IMMObject*> heapObjects;
    static unsigned long heapUsage;
    unsigned long heapAllocSize;
    bool bIsStackAllocated;

    static int newNum;
    static int deleteNum;
};

#define AUTO_SIZE unsigned long size(){return sizeof(*this);}

template <class T>
class CMMPointer
{
  protected:
    T* obj;
  public:
    CMMPointer()
    {
        obj = 0;
    }
    CMMPointer( T* o )
    {
        obj = 0; *this = o;
    }
    CMMPointer( const CMMPointer<T>& p )
    {
        obj = 0; *this = p;
    }

    ~CMMPointer()
    {
        if ( obj )
        {
          obj->Release();
        }
    }

    inline CMMPointer<T>& operator =( const CMMPointer<T>& p )
    {
        if ( obj )
        {
          obj->Release();
        }
        obj = p.obj;
        if ( obj )
        {
          obj->AddRef();
        }
        return *this;
    }
    inline CMMPointer<T>& operator =( T* o )
    {
        if ( obj )
        {
          obj->Release();
        }
        obj = o;
        if ( obj )
        {
          obj->AddRef();
        }
        return *this;
    }

    inline T& operator *() const
    {
        assert( obj != 0 && "Tried to * a NULL smart pointer" );
        return *obj;
    }

    inline T* operator ->() const
    {
        assert( obj != 0 && "Tried to -> on a NULL smart pointer" );
        return obj;
    }

    inline operator T *() const
    {
        return obj;
    }

    inline bool operator !()
    {
        return !( obj );
    }

    inline bool isValid() const
    {
        return ( obj != 0 );
    }

    inline bool operator ==( const CMMPointer<T>& p ) const
    {
        return ( obj == p.obj );
    }
    inline bool operator ==( const T* o ) const
    {
        return ( obj == o );
    }
};

template <class T, int i>
class CMMBlob : public IMMObject
{
  protected:
    T buffer[i];
  public:
    inline T& operator[]( int index )
    {
        assert( index < i ); return buffer[index];
    }
    inline operator T *()
    {
        return buffer;
    }
    AUTO_SIZE;
};

template <class T>
class CMMDynamicBlob : public IMMObject
{
  protected:
    unsigned long dataSize;
    T* buffer;
  public:
    inline T& operator[]( int index )
    {
        assert( index < dataSize ); return buffer[index];
    }
    inline operator T *()
    {
        return buffer;
    }
    CMMDynamicBlob( unsigned long size )
    {
        dataSize = size; buffer = new T[size];
    }
    ~CMMDynamicBlob()
    {
        if ( buffer )
        {
          delete[] buffer;
        }
    }
    unsigned long size()
    {
        return dataSize * sizeof( T ) + sizeof( this );
    }
    unsigned long blobsize()
    {
        return dataSize;
    }
};

#ifdef _USRDLL
//#include "kernel.h"
//void* IMMObject::operator new( size_t objsize )
//{
//  return KERNEL.CreateMMOObject( objsize );
//}
//
//void IMMObject::operator delete( void* obj )
//{
//  KERNEL.DeleteMMOObject( obj );
//}
#endif

#endif
