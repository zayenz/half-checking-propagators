// This file represents an import of the header-only library at https://github.com/oktal/result

/*
                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

   1. Definitions.

      "License" shall mean the terms and conditions for use, reproduction,
      and distribution as defined by Sections 1 through 9 of this document.

      "Licensor" shall mean the copyright owner or entity authorized by
      the copyright owner that is granting the License.

      "Legal Entity" shall mean the union of the acting entity and all
      other entities that control, are controlled by, or are under common
      control with that entity. For the purposes of this definition,
      "control" means (i) the power, direct or indirect, to cause the
      direction or management of such entity, whether by contract or
      otherwise, or (ii) ownership of fifty percent (50%) or more of the
      outstanding shares, or (iii) beneficial ownership of such entity.

      "You" (or "Your") shall mean an individual or Legal Entity
      exercising permissions granted by this License.

      "Source" form shall mean the preferred form for making modifications,
      including but not limited to software source code, documentation
      source, and configuration files.

      "Object" form shall mean any form resulting from mechanical
      transformation or translation of a Source form, including but
      not limited to compiled object code, generated documentation,
      and conversions to other media types.

      "Work" shall mean the work of authorship, whether in Source or
      Object form, made available under the License, as indicated by a
      copyright notice that is included in or attached to the work
      (an example is provided in the Appendix below).

      "Derivative Works" shall mean any work, whether in Source or Object
      form, that is based on (or derived from) the Work and for which the
      editorial revisions, annotations, elaborations, or other modifications
      represent, as a whole, an original work of authorship. For the purposes
      of this License, Derivative Works shall not include works that remain
      separable from, or merely link (or bind by name) to the interfaces of,
      the Work and Derivative Works thereof.

      "Contribution" shall mean any work of authorship, including
      the original version of the Work and any modifications or additions
      to that Work or Derivative Works thereof, that is intentionally
      submitted to Licensor for inclusion in the Work by the copyright owner
      or by an individual or Legal Entity authorized to submit on behalf of
      the copyright owner. For the purposes of this definition, "submitted"
      means any form of electronic, verbal, or written communication sent
      to the Licensor or its representatives, including but not limited to
      communication on electronic mailing lists, source code control systems,
      and issue tracking systems that are managed by, or on behalf of, the
      Licensor for the purpose of discussing and improving the Work, but
      excluding communication that is conspicuously marked or otherwise
      designated in writing by the copyright owner as "Not a Contribution."

      "Contributor" shall mean Licensor and any individual or Legal Entity
      on behalf of whom a Contribution has been received by Licensor and
      subsequently incorporated within the Work.

   2. Grant of Copyright License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      copyright license to reproduce, prepare Derivative Works of,
      publicly display, publicly perform, sublicense, and distribute the
      Work and such Derivative Works in Source or Object form.

   3. Grant of Patent License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      (except as stated in this section) patent license to make, have made,
      use, offer to sell, sell, import, and otherwise transfer the Work,
      where such license applies only to those patent claims licensable
      by such Contributor that are necessarily infringed by their
      Contribution(s) alone or by combination of their Contribution(s)
      with the Work to which such Contribution(s) was submitted. If You
      institute patent litigation against any entity (including a
      cross-claim or counterclaim in a lawsuit) alleging that the Work
      or a Contribution incorporated within the Work constitutes direct
      or contributory patent infringement, then any patent licenses
      granted to You under this License for that Work shall terminate
      as of the date such litigation is filed.

   4. Redistribution. You may reproduce and distribute copies of the
      Work or Derivative Works thereof in any medium, with or without
      modifications, and in Source or Object form, provided that You
      meet the following conditions:

      (a) You must give any other recipients of the Work or
          Derivative Works a copy of this License; and

      (b) You must cause any modified files to carry prominent notices
          stating that You changed the files; and

      (c) You must retain, in the Source form of any Derivative Works
          that You distribute, all copyright, patent, trademark, and
          attribution notices from the Source form of the Work,
          excluding those notices that do not pertain to any part of
          the Derivative Works; and

      (d) If the Work includes a "NOTICE" text file as part of its
          distribution, then any Derivative Works that You distribute must
          include a readable copy of the attribution notices contained
          within such NOTICE file, excluding those notices that do not
          pertain to any part of the Derivative Works, in at least one
          of the following places: within a NOTICE text file distributed
          as part of the Derivative Works; within the Source form or
          documentation, if provided along with the Derivative Works; or,
          within a display generated by the Derivative Works, if and
          wherever such third-party notices normally appear. The contents
          of the NOTICE file are for informational purposes only and
          do not modify the License. You may add Your own attribution
          notices within Derivative Works that You distribute, alongside
          or as an addendum to the NOTICE text from the Work, provided
          that such additional attribution notices cannot be construed
          as modifying the License.

      You may add Your own copyright statement to Your modifications and
      may provide additional or different license terms and conditions
      for use, reproduction, or distribution of Your modifications, or
      for any such Derivative Works as a whole, provided Your use,
      reproduction, and distribution of the Work otherwise complies with
      the conditions stated in this License.

   5. Submission of Contributions. Unless You explicitly state otherwise,
      any Contribution intentionally submitted for inclusion in the Work
      by You to the Licensor shall be under the terms and conditions of
      this License, without any additional terms or conditions.
      Notwithstanding the above, nothing herein shall supersede or modify
      the terms of any separate license agreement you may have executed
      with Licensor regarding such Contributions.

   6. Trademarks. This License does not grant permission to use the trade
      names, trademarks, service marks, or product names of the Licensor,
      except as required for reasonable and customary use in describing the
      origin of the Work and reproducing the content of the NOTICE file.

   7. Disclaimer of Warranty. Unless required by applicable law or
      agreed to in writing, Licensor provides the Work (and each
      Contributor provides its Contributions) on an "AS IS" BASIS,
      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
      implied, including, without limitation, any warranties or conditions
      of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
      PARTICULAR PURPOSE. You are solely responsible for determining the
      appropriateness of using or redistributing the Work and assume any
      risks associated with Your exercise of permissions under this License.

   8. Limitation of Liability. In no event and under no legal theory,
      whether in tort (including negligence), contract, or otherwise,
      unless required by applicable law (such as deliberate and grossly
      negligent acts) or agreed to in writing, shall any Contributor be
      liable to You for damages, including any direct, indirect, special,
      incidental, or consequential damages of any character arising as a
      result of this License or out of the use or inability to use the
      Work (including but not limited to damages for loss of goodwill,
      work stoppage, computer failure or malfunction, or any and all
      other commercial damages or losses), even if such Contributor
      has been advised of the possibility of such damages.

   9. Accepting Warranty or Additional Liability. While redistributing
      the Work or Derivative Works thereof, You may choose to offer,
      and charge a fee for, acceptance of support, warranty, indemnity,
      or other liability obligations and/or rights consistent with this
      License. However, in accepting such obligations, You may act only
      on Your own behalf and on Your sole responsibility, not on behalf
      of any other Contributor, and only if You agree to indemnify,
      defend, and hold each Contributor harmless for any liability
      incurred by, or claims asserted against, such Contributor by reason
      of your accepting any such warranty or additional liability.

   END OF TERMS AND CONDITIONS

   APPENDIX: How to apply the Apache License to your work.

      To apply the Apache License to your work, attach the following
      boilerplate notice, with the fields enclosed by brackets "{}"
      replaced with your own identifying information. (Don't include
      the brackets!)  The text should be enclosed in the appropriate
      comment syntax for the file format. We also recommend that a
      file or class name and description of purpose be included on the
      same "printed page" as the copyright notice for easier
      identification within third-party archives.

   Copyright {2016} {Mathieu Stefani}

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

/*
   Mathieu Stefani, 03 mai 2016

   This header provides a Result type that can be used to replace exceptions in code
   that has to handle error.

   Result<T, E> can be used to return and propagate an error to the caller. Result<T, E> is an algebraic
   data type that can either Ok(T) to represent success or Err(E) to represent an error.
*/

#pragma once

#include <iostream>
#include <functional>
#include <type_traits>

namespace types {
    template<typename T>
    struct Ok {
        Ok(const T& val) : val(val) { }
        Ok(T&& val) : val(std::move(val)) { }

        T val;
    };

    template<>
    struct Ok<void> { };

    template<typename E>
    struct Err {
        Err(const E& val) : val(val) { }
        Err(E&& val) : val(std::move(val)) { }

        E val;
    };
}

template<typename T, typename CleanT = typename std::decay<T>::type>
types::Ok<CleanT> Ok(T&& val) {
    return types::Ok<CleanT>(std::forward<T>(val));
}

inline types::Ok<void> Ok() {
    return types::Ok<void>();
}

template<typename E, typename CleanE = typename std::decay<E>::type>
types::Err<CleanE> Err(E&& val) {
    return types::Err<CleanE>(std::forward<E>(val));
}

template<typename T, typename E> struct Result;

namespace details {

    template<typename ...> struct void_t { typedef void type; };

    namespace impl {
        template<typename Func> struct result_of;

        template<typename Ret, typename Cls, typename... Args>
        struct result_of<Ret (Cls::*)(Args...)> : public result_of<Ret (Args...)> { };

        template<typename Ret, typename... Args>
        struct result_of<Ret (Args...)> {
            typedef Ret type;
        };
    }

    template<typename Func>
    struct result_of : public impl::result_of<decltype(&Func::operator())> { };

    template<typename Ret, typename Cls, typename... Args>
    struct result_of<Ret (Cls::*) (Args...) const> {
        typedef Ret type;
    };

    template<typename Ret, typename... Args>
    struct result_of<Ret (*)(Args...)> {
        typedef Ret type;
    };

    template<typename R>
    struct ResultOkType { typedef typename std::decay<R>::type type; };

    template<typename T, typename E>
    struct ResultOkType<Result<T, E>> {
        typedef T type;
    };

    template<typename R>
    struct ResultErrType { typedef R type; };

    template<typename T, typename E>
    struct ResultErrType<Result<T, E>> {
        typedef typename std::remove_reference<E>::type type;
    };

    template<typename R> struct IsResult : public std::false_type { };
    template<typename T, typename E>
    struct IsResult<Result<T, E>> : public std::true_type { };

    namespace ok {

        namespace impl {

            template<typename T> struct Map;

            template<typename Ret, typename Cls, typename Arg>
            struct Map<Ret (Cls::*)(Arg) const> : public Map<Ret (Arg)> { };

            template<typename Ret, typename Cls, typename Arg>
            struct Map<Ret (Cls::*)(Arg)> : public Map<Ret (Arg)> { };

// General implementation
            template<typename Ret, typename Arg>
            struct Map<Ret (Arg)> {

                static_assert(!IsResult<Ret>::value,
                              "Can not map a callback returning a Result, use andThen instead");

                template<typename T, typename E, typename Func>
                static Result<Ret, E> map(const Result<T, E>& result, Func func) {

                    static_assert(
                            std::is_same<T, Arg>::value ||
                            std::is_convertible<T, Arg>::value,
                            "Incompatible types detected");

                    if (result.isOk()) {
                        auto res = func(result.storage().template get<T>());
                        return types::Ok<Ret>(std::move(res));
                    }

                    return types::Err<E>(result.storage().template get<E>());
                }
            };

// Specialization for callback returning void
            template<typename Arg>
            struct Map<void (Arg)> {

                template<typename T, typename E, typename Func>
                static Result<void, E> map(const Result<T, E>& result, Func func) {

                    if (result.isOk()) {
                        func(result.storage().template get<T>());
                        return types::Ok<void>();
                    }

                    return types::Err<E>(result.storage().template get<E>());
                }
            };

// Specialization for a void Result
            template<typename Ret>
            struct Map<Ret (void)> {

                template<typename T, typename E, typename Func>
                static Result<Ret, E> map(const Result<T, E>& result, Func func) {
                    static_assert(std::is_same<T, void>::value,
                                  "Can not map a void callback on a non-void Result");

                    if (result.isOk()) {
                        auto ret = func();
                        return types::Ok<Ret>(std::move(ret));
                    }

                    return types::Err<E>(result.storage().template get<E>());
                }
            };

// Specialization for callback returning void on a void Result
            template<>
            struct Map<void (void)> {

                template<typename T, typename E, typename Func>
                static Result<void, E> map(const Result<T, E>& result, Func func) {
                    static_assert(std::is_same<T, void>::value,
                                  "Can not map a void callback on a non-void Result");

                    if (result.isOk()) {
                        func();
                        return types::Ok<void>();
                    }

                    return types::Err<E>(result.storage().template get<E>());
                }
            };

// General specialization for a callback returning a Result
            template<typename U, typename E, typename Arg>
            struct Map<Result<U, E> (Arg)> {

                template<typename T, typename Func>
                static Result<U, E> map(const Result<T, E>& result, Func func) {
                    static_assert(
                            std::is_same<T, Arg>::value ||
                            std::is_convertible<T, Arg>::value,
                            "Incompatible types detected");

                    if (result.isOk()) {
                        auto res = func(result.storage().template get<T>());
                        return res;
                    }

                    return types::Err<E>(result.storage().template get<E>());
                }
            };

// Specialization for a void callback returning a Result
            template<typename U, typename E>
            struct Map<Result<U, E> (void)> {

                template<typename T, typename Func>
                static Result<U, E> map(const Result<T, E>& result, Func func) {
                    static_assert(std::is_same<T, void>::value, "Can not call a void-callback on a non-void Result");

                    if (result.isOk()) {
                        auto res = func();
                        return res;
                    }

                    return types::Err<E>(result.storage().template get<E>());
                }

            };

        } // namespace impl

        template<typename Func> struct Map : public impl::Map<decltype(&Func::operator())> { };

        template<typename Ret, typename... Args>
        struct Map<Ret (*) (Args...)> : public impl::Map<Ret (Args...)> { };

        template<typename Ret, typename Cls, typename... Args>
        struct Map<Ret (Cls::*) (Args...)> : public impl::Map<Ret (Args...)> { };

        template<typename Ret, typename Cls, typename... Args>
        struct Map<Ret (Cls::*) (Args...) const> : public impl::Map<Ret (Args...)> { };

        template<typename Ret, typename... Args>
        struct Map<std::function<Ret (Args...)>> : public impl::Map<Ret (Args...)> { };

    } // namespace ok


    namespace err {

        namespace impl {

            template<typename T> struct Map;

            template<typename Ret, typename Cls, typename Arg>
            struct Map<Ret (Cls::*)(Arg) const> {

                static_assert(!IsResult<Ret>::value,
                              "Can not map a callback returning a Result, use orElse instead");

                template<typename T, typename E, typename Func>
                static Result<T, Ret> map(const Result<T, E>& result, Func func) {
                    if (result.isErr()) {
                        auto res = func(result.storage().template get<E>());
                        return types::Err<Ret>(res);
                    }

                    return types::Ok<T>(result.storage().template get<T>());
                }

                template<typename E, typename Func>
                static Result<void, Ret> map(const Result<void, E>& result, Func func) {
                    if (result.isErr()) {
                        auto res = func(result.storage().template get<E>());
                        return types::Err<Ret>(res);
                    }

                    return types::Ok<void>();
                }


            };

        } // namespace impl

        template<typename Func> struct Map : public impl::Map<decltype(&Func::operator())> { };

    } // namespace err;

    namespace And {

        namespace impl {

            template<typename Func> struct Then;

            template<typename Ret, typename... Args>
            struct Then<Ret (*)(Args...)> : public Then<Ret (Args...)> { };

            template<typename Ret, typename Cls, typename... Args>
            struct Then<Ret (Cls::*)(Args...)> : public Then<Ret (Args...)> { };

            template<typename Ret, typename Cls, typename... Args>
            struct Then<Ret (Cls::*)(Args...) const> : public Then<Ret (Args...)> { };

            template<typename Ret, typename Arg>
            struct Then<Ret (Arg)> {
                static_assert(std::is_same<Ret, void>::value,
                              "then() should not return anything, use map() instead");

                template<typename T, typename E, typename Func>
                static Result<T, E> then(const Result<T, E>& result, Func func) {
                    if (result.isOk()) {
                        func(result.storage().template get<T>());
                    }
                    return result;
                }
            };

            template<typename Ret>
            struct Then<Ret (void)> {
                static_assert(std::is_same<Ret, void>::value,
                              "then() should not return anything, use map() instead");

                template<typename T, typename E, typename Func>
                static Result<T, E> then(const Result<T, E>& result, Func func) {
                    static_assert(std::is_same<T, void>::value, "Can not call a void-callback on a non-void Result");

                    if (result.isOk()) {
                        func();
                    }

                    return result;
                }
            };


        } // namespace impl

        template<typename Func>
        struct Then : public impl::Then<decltype(&Func::operator())> { };

        template<typename Ret, typename... Args>
        struct Then<Ret (*) (Args...)> : public impl::Then<Ret (Args...)> { };

        template<typename Ret, typename Cls, typename... Args>
        struct Then<Ret (Cls::*)(Args...)> : public impl::Then<Ret (Args...)> { };

        template<typename Ret, typename Cls, typename... Args>
        struct Then<Ret (Cls::*)(Args...) const> : public impl::Then<Ret (Args...)> { };

    } // namespace And

    namespace Or {

        namespace impl {

            template<typename Func> struct Else;

            template<typename Ret, typename... Args>
            struct Else<Ret (*)(Args...)> : public Else<Ret (Args...)> { };

            template<typename Ret, typename Cls, typename... Args>
            struct Else<Ret (Cls::*)(Args...)> : public Else<Ret (Args...)> { };

            template<typename Ret, typename Cls, typename... Args>
            struct Else<Ret (Cls::*)(Args...) const> : public Else<Ret (Args...)> { };

            template<typename T, typename F, typename Arg>
            struct Else<Result<T, F> (Arg)> {

                template<typename E, typename Func>
                static Result<T, F> orElse(const Result<T, E>& result, Func func) {
                    static_assert(
                            std::is_same<E, Arg>::value ||
                            std::is_convertible<E, Arg>::value,
                            "Incompatible types detected");

                    if (result.isErr()) {
                        auto res = func(result.storage().template get<E>());
                        return res;
                    }

                    return types::Ok<T>(result.storage().template get<T>());
                }

                template<typename E, typename Func>
                static Result<void, F> orElse(const Result<void, E>& result, Func func) {
                    if (result.isErr()) {
                        auto res = func(result.storage().template get<E>());
                        return res;
                    }

                    return types::Ok<void>();
                }

            };

            template<typename T, typename F>
            struct Else<Result<T, F> (void)> {

                template<typename E, typename Func>
                static Result<T, F> orElse(const Result<T, E>& result, Func func) {
                    static_assert(std::is_same<T, void>::value,
                                  "Can not call a void-callback on a non-void Result");

                    if (result.isErr()) {
                        auto res = func();
                        return res;
                    }

                    return types::Ok<T>(result.storage().template get<T>());
                }

                template<typename E, typename Func>
                static Result<void, F> orElse(const Result<void, E>& result, Func func) {
                    if (result.isErr()) {
                        auto res = func();
                        return res;
                    }

                    return types::Ok<void>();
                }

            };

        } // namespace impl

        template<typename Func>
        struct Else : public impl::Else<decltype(&Func::operator())> { };

        template<typename Ret, typename... Args>
        struct Else<Ret (*) (Args...)> : public impl::Else<Ret (Args...)> { };

        template<typename Ret, typename Cls, typename... Args>
        struct Else<Ret (Cls::*)(Args...)> : public impl::Else<Ret (Args...)> { };

        template<typename Ret, typename Cls, typename... Args>
        struct Else<Ret (Cls::*)(Args...) const> : public impl::Else<Ret (Args...)> { };

    } // namespace Or

    namespace Other {

        namespace impl {

            template<typename Func> struct Wise;

            template<typename Ret, typename... Args>
            struct Wise<Ret (*)(Args...)> : public Wise<Ret (Args...)> { };

            template<typename Ret, typename Cls, typename... Args>
            struct Wise<Ret (Cls::*)(Args...)> : public Wise<Ret (Args...)> { };

            template<typename Ret, typename Cls, typename... Args>
            struct Wise<Ret (Cls::*)(Args...) const> : public Wise<Ret (Args...)> { };

            template<typename Ret, typename Arg>
            struct Wise<Ret (Arg)> {

                template<typename T, typename E, typename Func>
                static Result<T, E> otherwise(const Result<T, E>& result, Func func) {
                    static_assert(
                            std::is_same<E, Arg>::value ||
                            std::is_convertible<E, Arg>::value,
                            "Incompatible types detected");

                    static_assert(std::is_same<Ret, void>::value,
                                  "callback should not return anything, use mapError() for that");

                    if (result.isErr()) {
                        func(result.storage().template get<E>());
                    }
                    return result;
                }

            };

        } // namespace impl

        template<typename Func>
        struct Wise : public impl::Wise<decltype(&Func::operator())> { };

        template<typename Ret, typename... Args>
        struct Wise<Ret (*) (Args...)> : public impl::Wise<Ret (Args...)> { };

        template<typename Ret, typename Cls, typename... Args>
        struct Wise<Ret (Cls::*)(Args...)> : public impl::Wise<Ret (Args...)> { };

        template<typename Ret, typename Cls, typename... Args>
        struct Wise<Ret (Cls::*)(Args...) const> : public impl::Wise<Ret (Args...)> { };

    } // namespace Other

    template<typename T, typename E, typename Func,
            typename Ret =
            Result<
                    typename details::ResultOkType<
                            typename details::result_of<Func>::type
                    >::type,
                    E>
    >
    Ret map(const Result<T, E>& result, Func func) {
        return ok::Map<Func>::map(result, func);
    }

    template<typename T, typename E, typename Func,
            typename Ret =
            Result<T,
                    typename details::ResultErrType<
                            typename details::result_of<Func>::type
                    >::type
            >
    >
    Ret mapError(const Result<T, E>& result, Func func) {
        return err::Map<Func>::map(result, func);
    }

    template<typename T, typename E, typename Func>
    Result<T, E> then(const Result<T, E>& result, Func func) {
        return And::Then<Func>::then(result, func);
    }

    template<typename T, typename E, typename Func>
    Result<T, E> otherwise(const Result<T, E>& result, Func func) {
        return Other::Wise<Func>::otherwise(result, func);
    }

    template<typename T, typename E, typename Func,
            typename Ret =
            Result<T,
                    typename details::ResultErrType<
                            typename details::result_of<Func>::type
                    >::type
            >
    >
    Ret orElse(const Result<T, E>& result, Func func) {
        return Or::Else<Func>::orElse(result, func);
    }

    struct ok_tag { };
    struct err_tag { };

    template<typename T, typename E>
    struct Storage {
        static constexpr size_t Size = sizeof(T) > sizeof(E) ? sizeof(T) : sizeof(E);
        static constexpr size_t Align = sizeof(T) > sizeof(E) ? alignof(T) : alignof(E);

        typedef typename std::aligned_storage<Size, Align>::type type;

        Storage()
                : initialized_(false)
        { }

        void construct(types::Ok<T> ok)
        {
            new (&storage_) T(ok.val);
            initialized_ = true;
        }
        void construct(types::Err<E> err)
        {
            new (&storage_) E(err.val);
            initialized_ = true;
        }

        template<typename U>
        void rawConstruct(U&& val) {
            typedef typename std::decay<U>::type CleanU;

            new (&storage_) CleanU(std::forward<U>(val));
            initialized_ = true;
        }

        template<typename U>
        const U& get() const {
            return *reinterpret_cast<const U *>(&storage_);
        }

        template<typename U>
        U& get() {
            return *reinterpret_cast<U *>(&storage_);
        }

        void destroy(ok_tag) {
            if (initialized_) {
                get<T>().~T();
                initialized_ = false;
            }
        }

        void destroy(err_tag) {
            if (initialized_) {
                get<E>().~E();
                initialized_ = false;
            }
        }

        type storage_;
        bool initialized_;
    };

    template<typename E>
    struct Storage<void, E> {
        typedef typename std::aligned_storage<sizeof(E), alignof(E)>::type type;

        void construct(types::Ok<void>)
        {
            initialized_ = true;
        }

        void construct(types::Err<E> err)
        {
            new (&storage_) E(err.val);
            initialized_ = true;
        }

        template<typename U>
        void rawConstruct(U&& val) {
            typedef typename std::decay<U>::type CleanU;

            new (&storage_) CleanU(std::forward<U>(val));
            initialized_ = true;
        }

        void destroy(ok_tag) { initialized_ = false; }
        void destroy(err_tag) {
            if (initialized_) {
                get<E>().~E(); initialized_ = false;
            }
        }

        template<typename U>
        const U& get() const {
            return *reinterpret_cast<const U *>(&storage_);
        }

        template<typename U>
        U& get() {
            return *reinterpret_cast<U *>(&storage_);
        }

        type storage_;
        bool initialized_;
    };

    template<typename T, typename E>
    struct Constructor {

        static void move(Storage<T, E>&& src, Storage<T, E>& dst, ok_tag) {
            dst.rawConstruct(std::move(src.template get<T>()));
            src.destroy(ok_tag());
        }

        static void copy(const Storage<T, E>& src, Storage<T, E>& dst, ok_tag) {
            dst.rawConstruct(src.template get<T>());
        }

        static void move(Storage<T, E>&& src, Storage<T, E>& dst, err_tag) {
            dst.rawConstruct(std::move(src.template get<E>()));
            src.destroy(err_tag());
        }

        static void copy(const Storage<T, E>& src, Storage<T, E>& dst, err_tag) {
            dst.rawConstruct(src.template get<E>());
        }
    };

    template<typename E>
    struct Constructor<void, E> {
        static void move(Storage<void, E>&& src, Storage<void, E>& dst, ok_tag) {
        }

        static void copy(const Storage<void, E>& src, Storage<void, E>& dst, ok_tag) {
        }

        static void move(Storage<void, E>&& src, Storage<void, E>& dst, err_tag) {
            dst.rawConstruct(std::move(src.template get<E>()));
            src.destroy(err_tag());
        }

        static void copy(const Storage<void, E>& src, Storage<void, E>& dst, err_tag) {
            dst.rawConstruct(src.template get<E>());
        }
    };

} // namespace details

namespace concept {

    template<typename T, typename = void> struct EqualityComparable : std::false_type { };

    template<typename T>
    struct EqualityComparable<T,
            typename std::enable_if<
                    true,
                    typename details::void_t<decltype(std::declval<T>() == std::declval<T>())>::type
            >::type
    > : std::true_type
    {
    };


} // namespace concept

template<typename T, typename E>
struct Result {

    static_assert(!std::is_same<E, void>::value, "void error type is not allowed");

    typedef details::Storage<T, E> storage_type;

    Result(types::Ok<T> ok)
            : ok_(true)
    {
        storage_.construct(std::move(ok));
    }

    Result(types::Err<E> err)
            : ok_(false)
    {
        storage_.construct(std::move(err));
    }

    Result(Result&& other) {
        if (other.isOk()) {
            details::Constructor<T, E>::move(std::move(other.storage_), storage_, details::ok_tag());
            ok_ = true;
        } else {
            details::Constructor<T, E>::move(std::move(other.storage_), storage_, details::err_tag());
            ok_ = false;
        }
    }

    Result(const Result& other) {
        if (other.isOk()) {
            details::Constructor<T, E>::copy(other.storage_, storage_, details::ok_tag());
            ok_ = true;
        } else {
            details::Constructor<T, E>::copy(other.storage_, storage_, details::err_tag());
            ok_ = false;
        }
    }

    ~Result() {
        if (ok_)
            storage_.destroy(details::ok_tag());
        else
            storage_.destroy(details::err_tag());
    }

    bool isOk() const {
        return ok_;
    }

    bool isErr() const {
        return !ok_;
    }

    T expect(const char* str) const {
        if (!isOk()) {
            std::fprintf(stderr, "%s\n", str);
            std::terminate();
        }
        return expect_impl(std::is_same<T, void>());
    }

    template<typename Func,
            typename Ret =
            Result<
                    typename details::ResultOkType<
                            typename details::result_of<Func>::type
                    >::type,
                    E>
    >
    Ret map(Func func) const {
        return details::map(*this, func);
    }

    template<typename Func,
            typename Ret =
            Result<T,
                    typename details::ResultErrType<
                            typename details::result_of<Func>::type
                    >::type
            >
    >
    Ret mapError(Func func) const {
        return details::mapError(*this, func);
    }

    template<typename Func>
    Result<T, E> then(Func func) const {
        return details::then(*this, func);
    }

    template<typename Func>
    Result<T, E> otherwise(Func func) const {
        return details::otherwise(*this, func);
    }

    template<typename Func,
            typename Ret =
            Result<T,
                    typename details::ResultErrType<
                            typename details::result_of<Func>::type
                    >::type
            >
    >
    Ret orElse(Func func) const {
        return details::orElse(*this, func);
    }

    storage_type& storage() {
        return storage_;
    }

    const storage_type& storage() const {
        return storage_;
    }

    template<typename U = T>
    typename std::enable_if<
            !std::is_same<U, void>::value,
            U
    >::type
    unwrapOr(const U& defaultValue) const {
        if (isOk()) {
            return storage().template get<U>();
        }
        return defaultValue;
    }

    template<typename U = T>
    typename std::enable_if<
            !std::is_same<U, void>::value,
            U
    >::type
    unwrap() const {
        if (isOk()) {
            return storage().template get<U>();
        }

        std::fprintf(stderr, "Attempting to unwrap an error Result\n");
        std::terminate();
    }

    E unwrapErr() const {
        if (isErr()) {
            return storage().template get<E>();
        }

        std::fprintf(stderr, "Attempting to unwrapErr an ok Result\n");
        std::terminate();
    }

private:
    T expect_impl(std::true_type) const { }
    T expect_impl(std::false_type) const { return storage_.template get<T>(); }

    bool ok_;
    storage_type storage_;
};

template<typename T, typename E>
bool operator==(const Result<T, E>& lhs, const Result<T, E>& rhs) {
    static_assert(concept::EqualityComparable<T>::value, "T must be EqualityComparable for Result to be comparable");
    static_assert(concept::EqualityComparable<E>::value, "E must be EqualityComparable for Result to be comparable");

    if (lhs.isOk() && rhs.isOk()) {
        return lhs.storage().template get<T>() == rhs.storage().template get<T>();
    }
    if (lhs.isErr() && rhs.isErr()) {
        return lhs.storage().template get<E>() == rhs.storage().template get<E>();
    }
}

template<typename T, typename E>
bool operator==(const Result<T, E>& lhs, types::Ok<T> ok) {
    static_assert(concept::EqualityComparable<T>::value, "T must be EqualityComparable for Result to be comparable");

    if (!lhs.isOk()) return false;

    return lhs.storage().template get<T>() == ok.val;
}

template<typename E>
bool operator==(const Result<void, E>& lhs, types::Ok<void>) {
    return lhs.isOk();
}

template<typename T, typename E>
bool operator==(const Result<T, E>& lhs, types::Err<E> err) {
    static_assert(concept::EqualityComparable<E>::value, "E must be EqualityComparable for Result to be comparable");
    if (!lhs.isErr()) return false;

    return lhs.storage().template get<E>() == err.val;
}

#define TRY(...)                                                   \
    ({                                                             \
        auto res = __VA_ARGS__;                                    \
        if (!res.isOk()) {                                         \
            typedef details::ResultErrType<decltype(res)>::type E; \
            return types::Err<E>(res.storage().get<E>());          \
        }                                                          \
        typedef details::ResultOkType<decltype(res)>::type T;      \
        res.storage().get<T>();                                    \
    })
