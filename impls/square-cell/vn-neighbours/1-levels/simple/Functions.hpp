/*

Copyright (c) 2016 - Centre for Water Systems,
					 University of Exeter
				   - EAWAG - Swiss Federal Institute of Water Science
                           and Technology

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef _CA_FUNCTIONS_HPP_
#define _CA_FUNCTIONS_HPP_

//! \file Functions.hpp 
//! Contains the list of implementation specific Options
//! \author Michael J Gibson, University of Exeter, 
//! contact: m.j.gibson [at] exeter.ac.uk 
//! \date 2016-04


#if !defined __cplusplus
#error This caAPI  implementation requires a C++ compiler
#endif

namespace CA {


  //////////////////////////////// ONE ///////////////////////

  //! Function with one parameter other than CA_GRID.
  template <class F,class A1>
  struct Function1 
  {    
    Function1(F f, A1& a1): _f(f), _a1(a1) {}
    
    void operator()(_caGrid& cag)const { _f(cag,_a1); }

    F        _f;
    A1&      _a1;
  };

  //! Make a function with one parameter.
  template <class F,class A1>
  Function1<F,A1> makeFunction(F f, A1& a1)
  {
    return Function1<F,A1>(f,a1);
  }


  //////////////////////////////// TWO ///////////////////////

  //! Function with two parameters other than CA_GRID.
  template <class F,class A1, class A2>
  struct Function2 
  {    
    Function2(F f, A1& a1, A2& a2): _f(f),  _a1(a1), _a2(a2) {}
    
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
  };

  //! Make a function with two parameters.
  template <class F,class A1, class A2>
  Function2<F,A1,A2> makeFunction(F f,  A1& a1, A2& a2)
  {
    return Function2<F,A1,A2>(f,a1,a2);
  }


  //////////////////////////////// THREE ///////////////////////

  //! Function with three parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3>
  struct Function3 
  {    
    Function3(F f, A1& a1, A2& a2, A3& a3): _f(f), _a1(a1), _a2(a2), _a3(a3){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
  };

  //! Make a function with three parameters.
  template <class F,class A1, class A2, class A3>
  Function3<F,A1,A2,A3> makeFunction(F f, A1& a1, A2& a2, A3& a3)
  {
    return Function3<F,A1,A2,A3>(f,a1,a2,a3);
  }


  //////////////////////////////// FOUR ///////////////////////

  //! Function with four parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4>
  struct Function4 
  {    
    Function4(F f,  A1& a1, A2& a2, A3& a3, A4& a4): _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
  };

  //! Make a function with five parameters.
  template <class F,class A1, class A2, class A3, class A4>
  Function4<F,A1,A2,A3,A4> makeFunction(F f,  A1& a1, A2& a2, A3& a3, A4& a4)
  {
    return Function4<F,A1,A2,A3,A4>(f,a1,a2,a3,a4);
  }


  //////////////////////////////// FIVE ///////////////////////

  //! Function with five parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5>
  struct Function5 
  {    
    Function5(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
  };

  //! Make a function with five parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5>
  Function5<F,A1,A2,A3,A4,A5> makeFunction(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5)
  {
    return Function5<F,A1,A2,A3,A4,A5>(f,a1,a2,a3,a4,a5);
  }


  //////////////////////////////// SIX ///////////////////////

  //! Function with six parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6>
  struct Function6 
  {    
    Function6(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
  };

  //! Make a function with five parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6>
  Function6<F,A1,A2,A3,A4,A5,A6> makeFunction(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6)
  {
    return Function6<F,A1,A2,A3,A4,A5,A6>(f,a1,a2,a3,a4,a5,a6);
  }


  //////////////////////////////// SEVEN ///////////////////////

  //! Function with seven parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
  struct Function7 
  {    
    Function7(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
  };

  //! Make a function with five parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
  Function7<F,A1,A2,A3,A4,A5,A6,A7> makeFunction(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
						 A7& a7)
  {
    return Function7<F,A1,A2,A3,A4,A5,A6,A7>(f,a1,a2,a3,a4,a5,a6,a7);
  }

  //////////////////////////////// EIGHT ///////////////////////

  //! Function with eight parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
  struct Function8 
  {    
    Function8(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
	A8&      _a8;
  };

  //! Make a function with eight parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
  Function8<F,A1,A2,A3,A4,A5,A6,A7,A8> makeFunction(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
						 A7& a7, A8& a8)
  {
    return Function8<F,A1,A2,A3,A4,A5,A6,A7,A8>(f,a1,a2,a3,a4,a5,a6,a7,a8);
  }


  //////////////////////////////// NINE ///////////////////////

  //! Function with nine parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
  struct Function9 
  {    
    Function9(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8), _a9(a9){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
	A8&      _a8;
	A9&      _a9;
  };

  //! Make a function with nine parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
  Function9<F,A1,A2,A3,A4,A5,A6,A7,A8,A9> makeFunction(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
						 A7& a7, A8& a8, A9& a9)
  {
    return Function9<F,A1,A2,A3,A4,A5,A6,A7,A8,A9>(f,a1,a2,a3,a4,a5,a6,a7,a8,a9);
  }

  //////////////////////////////// TEN ///////////////////////

  //! Function with seven parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
  struct Function10 
  {    
    Function10(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9, A10& a10): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8), _a9(a9), _a10(a10){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
    A8&      _a8;
    A9&      _a9;
    A10&      _a10;
  };

  //! Make a function with eleven parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
  Function10<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> makeFunction(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
						 A7& a7, A8& a8, A9& a9, A10& a10)
  {
    return Function10<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
  }

  //////////////////////////////// ELEVEN ///////////////////////

  //! Function with seven parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11>
  struct Function11 
  {    
    Function11(F f,  A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9, A10& a10, A11& a11): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8), _a9(a9), _a10(a10), _a11(a11){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10,_a11); }

    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
    A8&      _a8;
    A9&      _a9;
    A10&      _a10;
    A11&      _a11;
  };

  //! Make a function with eleven parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11>
  Function11<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11> makeFunction(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
								A7& a7, A8& a8, A9& a9, A10& a10, A11& a11)
  {
    return Function11<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11>(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11);
  }


  //////////////////////////////// TWELVE ///////////////////////

  //! Function with seven parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12>
  struct Function12 
  {    
    Function12(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8), _a9(a9), _a10(a10), _a11(a11), _a12(a12){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10,_a11,_a12); }
    
    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
    A8&      _a8;
    A9&      _a9;
    A10&      _a10;
    A11&      _a11;
    A12&      _a12;
  };

  //! Make a function with twelve parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12>
  Function12<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12> makeFunction(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
								    A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12)
  {
    return Function12<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12>(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12);
  }


  //////////////////////////////// THIRTEEN ///////////////////////

  //! Function with seven parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13>
  struct Function13
  {    
    Function13(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8), _a9(a9), _a10(a10), _a11(a11), _a12(a12), _a13(a13){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10,_a11,_a12,_a13); }
    
    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
    A8&      _a8;
    A9&      _a9;
    A10&      _a10;
    A11&      _a11;
    A12&      _a12;
    A13&      _a13;
  };

  //! Make a function with thirteen parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13>
  Function13<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13> makeFunction(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
									A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13)
  {
    return Function13<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13>(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13);
  }


//////////////////////////////// FOURTHENN ///////////////////////

  //! Function with seven parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14>
  struct Function14
  {    
    Function14(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8), _a9(a9), _a10(a10), _a11(a11), _a12(a12), _a13(a13), _a14(a14){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10,_a11,_a12,_a13,_a14); }
    
    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
    A8&      _a8;
    A9&      _a9;
    A10&      _a10;
    A11&      _a11;
    A12&      _a12;
    A13&      _a13;
    A14&      _a14;
  };

  //! Make a function with fourthenn parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14>
  Function14<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14> makeFunction(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
									    A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14)
  {
    return Function14<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14>(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14);
  }


//////////////////////////////// 15 ///////////////////////

  //! Function with seven parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15>
  struct Function15
  {    
    Function15(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8), _a9(a9), _a10(a10), _a11(a11), _a12(a12), _a13(a13), _a14(a14), _a15(a15){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10,_a11,_a12,_a13,_a14,_a15); }
    
    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
    A8&      _a8;
    A9&      _a9;
    A10&      _a10;
    A11&      _a11;
    A12&      _a12;
    A13&      _a13;
    A14&      _a14;
    A15&      _a15;
  };

  //! Make a function with fourthenn parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15>
  Function15<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15> makeFunction(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
										A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15)
  {
    return Function15<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15>(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15);
  }


//////////////////////////////// 16 ///////////////////////

  //! Function with seven parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15, class A16>
  struct Function16
  {    
    Function16(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15, A16& a16): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8), _a9(a9), _a10(a10), _a11(a11), _a12(a12), _a13(a13), _a14(a14), _a15(a15), _a16(a16){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10,_a11,_a12,_a13,_a14,_a15,_a16); }
    
    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
    A8&      _a8;
    A9&      _a9;
    A10&      _a10;
    A11&      _a11;
    A12&      _a12;
    A13&      _a13;
    A14&      _a14;
    A15&      _a15;
    A16&      _a16;
  };

  //! Make a function with fourthenn parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15, class A16>
  Function16<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16> makeFunction(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
										    A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15, A16& a16)
  {
    return Function16<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16>(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15, a16);
  }

  //////////////////////////////// 17 ///////////////////////

  //! Function with seven parameters other than CA_GRID.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15, class A16, class A17>
  struct Function17
  {    
    Function17(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15, A16& a16, A17& a17): 
      _f(f),  _a1(a1), _a2(a2), _a3(a3), _a4(a4), _a5(a5), _a6(a6), _a7(a7), _a8(a8), _a9(a9), _a10(a10), _a11(a11), _a12(a12), _a13(a13), _a14(a14), _a15(a15), _a16(a16), _a17(a17){}
 
    void operator()(_caGrid& cag)const { _f(cag,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10,_a11,_a12,_a13,_a14,_a15,_a16,_a17); }
    
    F        _f;
    A1&      _a1;
    A2&      _a2;
    A3&      _a3;
    A4&      _a4;
    A5&      _a5;
    A6&      _a6;
    A7&      _a7;
    A8&      _a8;
    A9&      _a9;
    A10&      _a10;
    A11&      _a11;
    A12&      _a12;
    A13&      _a13;
    A14&      _a14;
    A15&      _a15;
    A16&      _a16;
    A17&      _a17;
  };

  //! Make a function with fourthenn parameters.
  template <class F,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15, class A16, class A17>
  Function17<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17> makeFunction(F f, A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, 
										    A7& a7, A8& a8, A9& a9, A10& a10, A11& a11, A12& a12, A13& a13, A14& a14, A15& a15, A16& a16, A17& a17)
  {
    return Function17<F,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17>(f,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15, a16,a17);
  }
}

#endif	// _CA_HPP_
