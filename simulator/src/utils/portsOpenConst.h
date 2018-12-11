/*******************************************************************************
 * Copyright (C) 2018 Jan Moritz Joseph
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#pragma once

#include "systemc.h"

template<typename T>
sc_core::sc_signal_in_if<T> const&
portConst(T const& v) // keep the name consistent with vh_open
{
    // Yes, this is an (elaboration-time) memory leak.  You can avoid it with some extra effort
    sc_core::sc_signal<T>* sig_p = new sc_core::sc_signal<T>(
            sc_core::sc_gen_unique_name("portConst"));
    sig_p->write(v);
    return *sig_p;
}

static struct {
    template<typename T>
    operator sc_core::sc_signal_inout_if<T>&() const
    {
        return *(new sc_core::sc_signal<T>(
                sc_core::sc_gen_unique_name("portOpen")));
    }
} const portOpen = {};
