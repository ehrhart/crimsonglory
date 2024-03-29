#include "engine.h"
#include "misc.h"

IMMObject* IMMObject::liveObjects = 0;
IMMObject* IMMObject::deadObjects = 0;
std::list<IMMObject*> IMMObject::heapObjects;
unsigned long IMMObject::heapUsage = 0;
int IMMObject::newNum = 0;
int IMMObject::deleteNum = 0;

IMMObject::IMMObject()
{
    nextObject = prevObject = 0;
    refCount = 0;
    std::list<IMMObject*>::iterator it = std::find( heapObjects.begin(), heapObjects.end(), this );
    if ( it == heapObjects.end() )
    {
      bIsStackAllocated = true;
    }
    else
    {
      bIsStackAllocated = false;
      heapObjects.erase( it );
    }
    if ( !bIsStackAllocated )
    {
      //start on the deadObjects list
      nextObject = deadObjects;
      if ( deadObjects )
      {
        deadObjects->prevObject = this;
      }
      deadObjects = this;
    }
}

IMMObject::~IMMObject()
{
}

void IMMObject::CollectGarbage()
{
    //while ( deadObjects )
    //{
    //    IMMObject* nObj = deadObjects->nextObject;
    //    delete deadObjects;
    //    deadObjects = nObj;
    //}
    while ( deadObjects )
    {
      IMMObject* obj = deadObjects;
      deadObjects = deadObjects->nextObject;
      delete obj;
    }
}

void IMMObject::AddRef()
{
    refCount++;
    if ( !bIsStackAllocated && ( refCount == 1 ) )
    {
      //move to the liveObjects list
      if ( prevObject )
      {
        prevObject->nextObject = nextObject;
      }
      if ( nextObject )
      {
        nextObject->prevObject = prevObject;
      }
      if ( deadObjects == this )
      {
        deadObjects = nextObject;
      }
      prevObject = 0;
      nextObject = liveObjects; 
      if ( liveObjects )
      {
        liveObjects->prevObject = this;
      }
      liveObjects = this;
    }
}

void IMMObject::Release()
{
    refCount--;
    if ( !bIsStackAllocated && ( refCount <= 0 ) )
    {
      //remove self from live list
      if ( prevObject )
      {
        prevObject->nextObject = nextObject;
      }
      if ( nextObject )
      {
        nextObject->prevObject = prevObject;
      }
      if ( liveObjects == this )
      {
        liveObjects = nextObject;
      }
      prevObject = 0;
      //add self to dead list
      nextObject = deadObjects;
      if ( deadObjects )
      {
        deadObjects->prevObject = this;
      }
      deadObjects = this;
    }
}

void IMMObject::CollectRemainingObjects( bool bEmitWarnings )
{
    CollectGarbage();
    while ( liveObjects )
    {
      IMMObject* o = liveObjects->nextObject;
      if ( bEmitWarnings )
      {
        //copy the object to a temporary buffer so that our '10 bytes' message doesn't
        //cause an access violation
        char szBuf[11]; memset( szBuf, 0, 11 );
        memcpy( szBuf, liveObjects, min( liveObjects->size(), 10 ) );
        APPLOG.Write( "Unreleased MMObject at %#X, length %u bytes, first bytes ""%.10s""", liveObjects, liveObjects->size(), szBuf );
      }
      delete liveObjects;
      liveObjects = o;
    }

    if ( bEmitWarnings )
    {
      APPLOG.Write( "MManager: objects created %i; deleted %i; difference %i", newNum, deleteNum, newNum - deleteNum );
    }
}

void* IMMObject::operator new( size_t objsize )
{
    newNum++;
    void* newObj = malloc( objsize );
    heapUsage += objsize;
    heapObjects.push_back( ( IMMObject * )newObj );
    ( ( IMMObject * )newObj )->heapAllocSize = objsize;
    return newObj;
}

void IMMObject::operator delete( void* obj )
{
    if ( !( ( IMMObject * )obj )->bIsStackAllocated )
    {
      deleteNum++;
      heapUsage -= ( ( IMMObject * )obj )->heapAllocSize;
      free( obj );
    }
}