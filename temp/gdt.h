#ifndef __GDE_H
#define __GDE_H

#include "types.h"

    class GlobalDescriptorTable{
        
        public:
            class SegmentDescripter
            {
                private:
                    uint16_t limit_lo;
                    uint16_t base_lo;
                    uint8_t base_hi;
                    uint8_t type;
                    uint8_t flags_limit_hi;
                    uint8_t base_vhi;

                public:
                    SegmentDescripter(uint32_t base, uint32_t limit, uint8_t type);
                    uint32_t Base();
                    uint32_t Limit();

            } __attribute__((packed));

            SegmentDescripter nullSegmentSelector;
            SegmentDescripter unusedSegmentSelector;
            SegmentDescripter codeSegmentSelector;
            SegmentDescripter dataSegmentSelector;

       public:
            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            uint16_t CodeSegmentSelector();
            uint16_t DataSegmentSelector();
    };

#endif
