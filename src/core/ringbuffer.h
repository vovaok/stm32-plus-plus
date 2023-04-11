#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

template<typename T>
class RingBuffer
{
public:
    RingBuffer(int size) :
        m_data(new T[size]),
        m_head(m_data),
        m_tail(m_data),
        m_last(m_data + size - 1),
        m_size(size)
    {
    }
    
    RingBuffer(const RingBuffer<T> &other) :
        m_data(new T[other.m_size]),
        m_head(m_data),
        m_tail(m_data),
        m_last(m_data + other.m_size - 1),
        m_size(other.m_size)
    {
        for (const T *src = other.m_tail; src != other.m_head; src++)
            push_back(*src);
    }
    
    RingBuffer &operator =(const RingBuffer<T> &other)
    {
        if (m_data)
            delete [] m_data;
        m_data = new T[other.m_size],
        m_head = m_tail = m_data;
        m_last = m_data + other.m_size - 1;
        m_size = other.m_size;
        for (const T *src = other.m_tail; src != other.m_head; src++)
            push_back(*src);
        return *this;
    }
    
    RingBuffer(RingBuffer<T> &&other) :
        m_data(other.m_data),
        m_head(other.m_head),
        m_tail(other.m_tail),
        m_last(other.m_last),
        m_size(other.m_size)
    {
        other.m_data = 0L;
        other.m_head = other.m_tail = other.m_last = 0L;
        other.m_size = 0;
    }
    
    RingBuffer &operator =(RingBuffer<T> &&other)
    {
        if (m_data)
            delete [] m_data;
        m_data = other.m_data;
        m_head = other.m_head;
        m_tail = other.m_tail;
        m_last = other.m_last;
        m_size = other.m_size;
        other.m_data = 0L;
        other.m_head = other.m_tail = other.m_last = 0L;
        other.m_size = 0;
        return *this;
    }
    
    ~RingBuffer()
    {
        delete [] m_data;
    }
    
    void resize(int new_size)
    {
        T *old_data = m_data;
        T *new_data = new T[new_size];
        /// @todo implement the copying of the data if needed
        delete [] old_data;
        
        m_data = new_data;
        m_head = m_data;
        m_tail = m_data;
        m_last = m_data + new_size - 1;
        m_size = new_size;
    }
    
    bool isEmpty() const {return m_head == m_tail;}
    
    void clear()
    {
        m_tail = m_head;
    }
    
    int size() const
    {
        int sz = m_head - m_tail;
        if (sz < 0)
            sz += m_size;
        return sz;
    }
    
    int maxsize() const {return m_size - 1;}
    
    int cont_size() const // maximum count of contiguous elements
    {
        if (m_head < m_tail)
            return m_last - m_head + 1;
        return m_head - m_tail;
    }
    
    void push_back(const T &value)
    {
        if (m_head == m_last)
        {
            if (m_tail == m_data)
                m_tail++; // discard last element
            *m_head = value;
            m_head = m_data;
            return;
        }
        *m_head++ = value;
        if (m_head == m_tail)
        {
            if (m_tail == m_last)
                m_tail = m_data;
            else
                m_tail++;
        }
    }
    
    void push_front(const T &value)
    {
        if (m_tail == m_data)
        {
            if (m_head == m_last)
                --m_head; // discard last element
            m_tail = m_last;
            *m_tail = value;
            return;
        }
        *--m_tail = value;
        if (m_head == m_tail)
        {
            if (m_head == m_data)
                m_head = m_last;
            else
                --m_head;
        }
    }
    
    void pop_back()
    {
        if (m_head == m_tail)
            return;
        if (m_head == m_data)
            m_head = m_last;
        else
            --m_head;
    }
    
    void pop_front()
    {
        if (m_head == m_tail)
            return;
        if (m_tail >= m_last)
            m_tail = m_data;
        else
            m_tail++;
    }
    
    const T *data() {return m_data;}
    const T &front() {return *m_tail;}
    const T &back() {return *m_head;}
    
    T take_front()
    {
        T value = *m_tail;
        pop_front();
        return value;
    }
    
    T take_back()
    {
        pop_back();
        return *m_head;
    }
    
private:
    T *m_data, *m_head, *m_tail, *m_last;
    int m_size;
};

#endif