#pragma once
/*!
  @file   utilities.h
  @brief  Various helper functions: endian-ness conversions, logging
*/

#include <stdint.h>
#include <Arduino.h>
#include <Ethernet.h>
#include "AR488_ComPorts.h"

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

/*!
  @brief  Manages storage and conversion of 4-byte big-endian data.

  The WiFi packets used by RPC_Server and VXI_Server transmit
  data in big-endian format; however, the AVR4089 and C++
  use little-endian format. When a packet is read into a buffer
  which has been described as a series of big_endian_32_t members,
  this class allows automatic conversion from the big-endian
  packet data to a little-endian C++ variable and vice-versa.
*/
class big_endian_32_t
{
  private:
    uint32_t b_e_data; ///< Data storage is treated as uint32_t, but the data in this member is actually big-endian

  public:
    /*!
      @brief  Constructor takes a uint32_t and stores it in big-endian form.

      @param  data  little-endian data to be converted and stored as big-endian
    */
    big_endian_32_t(uint32_t data) { b_e_data = htonl(data); }

    /*!
      @brief  Implicit or explicit conversion to little-endian uint32_t.

      @return Little-endian equivalent of big-endian data stored in m_data
    */
    operator uint32_t() { return ntohl(b_e_data); }
};

/*!
  @brief  4-byte integer that cycles through a defined range.

  The cyclic_uint32_t class allows storage, retrieval,
  and increment/decrement of a value that must cycle
  through a constrained range. When an increment or
  decrement exceeds the limits of the range, the
  value cycles to the opposite end of the range.
*/
class cyclic_uint32_t
{
  private:
    uint32_t m_data;
    uint32_t m_start;
    uint32_t m_end;

  public:
    /*!
      @brief  The constructor requires the range start and end.

      Initial value may optionally be included as well; if it is
      not included, the value will default to the start of the range.
    */
    cyclic_uint32_t(uint32_t start, uint32_t end, uint32_t value = 0)
    {
        m_start = start < end ? start : end;
        m_end = start < end ? end : start;
        m_data = value >= m_start && value <= m_end ? value : m_start;
    }

    /*!
      @return True if the counter is not cyclic in reality.
    */
    bool is_noncyclic() { return m_start == m_end; }

    /*!
      @brief  Cycle to the previous value.

      If the current value is at the start of the range, the
      "previous value" will loop around to the end of the range.
      Otherwise, the "previous value" is simply the current
      value - 1.

      @return The previous value.
    */
    uint32_t goto_prev()
    {
        m_data = m_data > m_start ? m_data - 1 : m_end;
        return m_data;
    }

    /*!
      @brief  Cycle to the next value.

      If the current value is at the end of the range, the
      "next value" will loop around to the start of the range.
      Otherwise, the "next value" is simply the current value + 1.

      @return The next value.
    */
    uint32_t goto_next()
    {
        m_data = m_data < m_end ? m_data + 1 : m_start;
        return m_data;
    }

    /*!
      @brief  Pre-increment operator, e.g., ++port.

      @return The next value (the value after cyling forward).
    */
    uint32_t operator++()
    {
        return goto_next();
    }

    /*!
      @brief  Post-increment operator, e.g., port++.

      @return The current value (the value before cyling forward).
    */
    uint32_t operator++(int)
    {
        uint32_t temp = m_data;
        goto_next();
        return temp;
    }

    /*!
      @brief  Pre-decrement operator, e.g., --port.

      @return The previous value (the value after cyling backward).
    */
    uint32_t operator--()
    {
        return goto_prev();
    }

    /*!
      @brief  Post-decrement operator, e.g., port--.

      @return The current value (the value before cyling backward).
    */
    uint32_t operator--(int) // postfix version
    {
        uint32_t temp = m_data;
        goto_prev();
        return temp;
    }

    /*!
      @brief  Allows conversion to uint32_t.

      Example:
      @code
        regular_uint32_t = cyclic_uint32_t_instance();
      @endcode

      @return The current value.
    */
    uint32_t operator()()
    {
        return m_data;
    }

    /*!
      @brief  Implicit conversion or explicit cast to uint32_t.

      Example:
      @code
        regular_uint32_t = (uint32_t)cyclic_uint32_t_instance;
      @endcode

      @return The current value.
    */
    operator uint32_t()
    {
        return m_data;
    }
};

