#include <memorymanagement.h>

using namespace myos;
using namespace myos::common;

MemoryManager* MemoryManager::activeMemoryManager = 0;

MemoryManager::MemoryManager(size_t first, size_t size){
    activeMemoryManager = this;
    
    if(size < sizeof(MemoryChunk)){
        this->first = 0;
    }else{
        this->first = (MemoryChunk*)first;
        this->first->allocated = false;
        this->first->prev = 0;
        this->first->next = 0;
        this->first->size = size - sizeof(MemoryChunk);
    }
    

}
MemoryManager::~MemoryManager(){
    if(activeMemoryManager == this)
        activeMemoryManager = 0;

}

void* MemoryManager::malloc(size_t size){
    MemoryChunk* result = 0;
    for(MemoryChunk* chunk = first; chunk != 0 && result == 0; chunk = chunk->next){
        if(chunk->size > size && !chunk->allocated){
            result = chunk;
        }
    }
    if(!result)
        return 0;
    if(result->size >= size + sizeof(MemoryChunk) + 1){
        MemoryChunk* temp = (MemoryChunk*)((size_t) result + sizeof(MemoryChunk) + size);
        temp->allocated = false;
        temp->size = result->size - size - sizeof(MemoryChunk);
        temp->prev = result;
        temp->next = result->next;
        if(temp->next){
            temp->next->prev = temp;
        }
        result->next = temp;
        result->size = size;

    }
    result->allocated = true;
    return (void*)(((size_t)result) + sizeof(MemoryChunk));
}

void MemoryManager::free(void* ptr){
    MemoryChunk* chunk = (MemoryChunk*) ((size_t)ptr - sizeof(MemoryChunk));
    chunk->allocated = false;
    if(chunk->prev != 0 && !chunk->prev->allocated){
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        if(chunk->next){
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }
    if(chunk->next != 0 && !chunk->next->allocated){
        chunk->next = chunk->next->next;
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        if(chunk->next->next){
            chunk->next->next->prev = chunk;
        }
    }
}


void* operator new(common::size_t size){
    if(myos::MemoryManager::activeMemoryManager)
        return myos::MemoryManager::activeMemoryManager->malloc(size);
    return 0;
}
void* operator new[](common::size_t size){
    if(myos::MemoryManager::activeMemoryManager)
        return myos::MemoryManager::activeMemoryManager->malloc(size);
    return 0;
}

void* operator new(common::size_t size, void* ptr){
    return ptr;
}

void* operator new[](common::size_t size, void* ptr){
    return ptr;
}

void operator delete(void* ptr){
    if(myos::MemoryManager::activeMemoryManager)
        myos::MemoryManager::activeMemoryManager->free(ptr);   
}
void operator delete[](void* ptr){
    if(myos::MemoryManager::activeMemoryManager)
        myos::MemoryManager::activeMemoryManager->free(ptr);   
}
