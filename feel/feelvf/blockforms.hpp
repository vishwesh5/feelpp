/* -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t  -*-

 This file is part of the Feel++ library

 Author(s): Christophe Prud'homme <christophe.prudhomme@feelpp.org>
 Date: 24 Jul 2016

 Copyright (C) 2016 Feel++ Consortium

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef FEELPP_BLOCKFORMS_HPP
#define FEELPP_BLOCKFORMS_HPP 1

#include <feel/feelvf/form.hpp>
#include <feel/feelalg/vectorblock.hpp>
#include <feel/feeldiscr/product.hpp>


namespace Feel {

/**
 * Handles bilinear form over a product of spaces
 */
template<typename PS>
class BlockBilinearForm
{
public :
    using product_space_t = PS;
    using value_type = double;
    BlockBilinearForm(product_space_t&& ps )
        :
        M_ps(ps),
        M_matrix(backend()->newBlockMatrix(_block=csrGraphBlocks(M_ps)))
        {}
    BlockBilinearForm(product_space_t&& ps, size_type pattern )
        :
        M_ps(ps),
        M_matrix(backend()->newBlockMatrix(_block=csrGraphBlocks(M_ps, pattern)))
        {}
    BlockBilinearForm(product_space_t&& ps, sparse_matrix_ptrtype m)
        :
        M_ps(ps),
        M_matrix(m)
        {}
    BlockBilinearForm& operator+=( BlockBilinearForm& a )
        {
            if ( this == &a )
                return *this;

            M_matrix->addMatrix( 1.0, a.M_matrix );

            return *this;
        }
#if 0
    template<typename N1,typename N2>
    decltype(auto) operator()( N1 n1, N2 n2 )
        {
            cout << "filling out matrix block (" << n1 << "," << n2 << ")\n";

        }
#endif
    template<typename N1, typename N2>
    decltype(auto) operator()( N1 n1, N2 n2, int s1 = 0, int s2 = 0 )
        {
            int n = 0;
            auto&& spaces=remove_shared_ptr_f( M_ps );
            #if 0
            hana::if_( hana::bool_<Feel::is_shared_ptr_v<PS>>{},
                                     []( auto&& x ) { return *x; },
                                     []( auto&& x ) { return x; } )(M_ps);
            #endif
            auto test_space = hana::at( spaces, n1 );
            auto trial_space = hana::at( spaces, n2 );

            return hana::eval_if(std::is_base_of<ProductSpaceBase,decay_type<decltype(test_space)>>{},
                                 [&]( auto _ ) { return hana::eval_if( std::is_base_of<ProductSpaceBase,decay_type<decltype(trial_space)>>{},
                                                                 [&] (auto _) {
                                                                     cout << "filling out dyn matrix block (" << int(n1) + s1 << "," << int(n2)+s2  << ")\n";
                                                                     return form2(_test=(*_(test_space))[s1],_trial=(*_(trial_space))[s2],
                                                                                  _name="bilinearform.a"s+"("+std::to_string(int(n1)+s1)+","s+std::to_string(int(n2)+s2)+")"s,
                                                                                  _matrix=M_matrix->block(int(n1)+s1,int(n2)+s2), _rowstart=int(n1)+s1, _colstart=int(n2)+s2 );
                                                                 },
                                                                 [&] (auto _){
                                                                     cout << "filling out dyn matrix block (" << int(n1) + s1 << "," << int(n2)+s2  << ")\n";
                                                                     return form2(_test=(*_(test_space))[s1],_trial=_(trial_space),
                                                                                  _name="bilinearform.a"s+"("+std::to_string(int(n1)+s1)+","s+std::to_string(int(n2))+")"s,
                                                                                  _matrix=M_matrix->block(int(n1)+s1,int(n2)), _rowstart=int(n1)+s1, _colstart=int(n2) );
                                                                 }); },
                                 [&]( auto _ ) { return hana::eval_if( std::is_base_of<ProductSpaceBase,decay_type<decltype(trial_space)>>{},
                                                                 [&] (auto _) {
                                                                     cout << "filling out dyn matrix block (" << int(n1) + s1 << "," << int(n2)+s2  << ")\n";
                                                                     return form2(_test=_(test_space),_trial=(*_(trial_space))[s2],
                                                                                  _name="bilinearform.a"s+"("+std::to_string(int(n1))+","s+std::to_string(int(n2)+s2)+")"s,
                                                                                  _matrix=M_matrix->block(int(n1),int(n2)+s2), _rowstart=int(n1), _colstart=int(n2)+s2 );
                                                                 },
                                                                 [&] (auto _){
                                                                     cout << "filling out dyn matrix block (" << int(n1) + s1 << "," << int(n2)+s2  << ")\n";
                                                                     return form2(_test=_(test_space),_trial=_(trial_space),
                                                                                  _name="bilinearform.a"s+"("+std::to_string(int(n1))+","s+std::to_string(int(n2))+")"s,
                                                                                  _matrix=M_matrix->block(int(n1),int(n2)), _rowstart=int(n1), _colstart=int(n2) );
                                                                 }); });


        }

    decltype(auto) operator()( int n1, int n2 )
        {
            cout << "filling out matrix block (" << n1 << "," << n2 << ")\n";
            return form2(_test=M_ps[n1],_trial=M_ps[n2], _matrix=M_matrix, _rowstart=int(n1), _colstart=int(n2) );
        }

    void close()
        {
            M_matrix->close();
        }
    void zero()
        {
            M_matrix->zero();
        }
    void syncLocalMatrix()
        {
            int s = M_ps.numberOfSpaces();
            int n = 0;
            auto cp = hana::cartesian_product( hana::make_tuple( M_ps, M_ps ) );
            int nstatic = hana::if_(std::is_base_of<ProductSpaceBase,decay_type<decltype(hana::back(M_ps))>>{},
                                    [s] (auto&& x ) { return s-hana::back(std::forward<decltype(x)>(x))->numberOfSpaces()+1; },
                                    [s] (auto&& x ) { return s; } )( M_ps );
            hana::for_each( cp, [&]( auto const& e )
                            {
                                int r = n/nstatic;
                                int c = n%nstatic;
                                auto test_space = e[0_c];
                                auto trial_space = e[1_c];
                                DVLOG( 1 ) << "syncLocalMatrix("<< r << ","<< c <<")dim "<< test_space->mesh()->dimension()<<" , " << trial_space->mesh()->dimension() <<"\n";
                                M_matrix->block(r,c)->sc()->syncLocalMatrix( test_space,trial_space );
                                ++n;
                            });
        }
    sparse_matrix_ptrtype matrixPtr() const { return M_matrix; }
    sparse_matrix_ptrtype matrixPtr() { return M_matrix; }
    using pre_solve_type = typename Backend<value_type>::pre_solve_type;
    using post_solve_type = typename Backend<value_type>::post_solve_type;
    BOOST_PARAMETER_MEMBER_FUNCTION( ( typename Backend<double>::solve_return_type ),
                                     solve,
                                     tag,
                                     ( required
                                       ( in_out( solution ),* )
                                       ( rhs, * ) )
                                     ( optional
                                       ( condense,           ( bool ), false )
                                       ( name,           ( std::string ), "" )
                                       ( kind,           ( std::string ), soption(_prefix=name,_name="backend") )
                                       ( rebuild,        ( bool ), boption(_prefix=name,_name="backend.rebuild") )
                                       ( pre, (pre_solve_type), pre_solve_type() )
                                       ( post, (post_solve_type), post_solve_type() )
                                       ) )
        {
            
            if ( condense &&  hana::Foldable<PS>::value )
                return solveImplCondense( M_ps, solution, rhs, name, kind, rebuild, pre, post );
            return solveImpl( solution, rhs, name, kind, rebuild, pre, post );
                
        }
    template <typename PS_t, typename Solution_t, typename Rhs_t>
    typename Backend<double>::solve_return_type
    solveImplCondense( PS_t& ps, Solution_t& solution, Rhs_t const& rhs, std::string const& name, std::string const& kind,
                       bool rebuild, pre_solve_type pre, post_solve_type post,
                       std::enable_if_t<std::is_base_of<ProductSpaceBase,decay_type<PS_t>>::value>* = nullptr )
        {
            return solveImpl( solution, rhs, name, kind, rebuild, pre, post );
        }
    template <typename PS_t, typename Solution_t, typename Rhs_t>
    typename Backend<double>::solve_return_type
    solveImplCondense( PS_t& ps, Solution_t& solution, Rhs_t const& rhs, std::string const& name, std::string const& kind,
                       bool rebuild, pre_solve_type pre, post_solve_type post,
                       std::enable_if_t< hana::Foldable<PS_t>::value &&
                                         !std::is_base_of<ProductSpaceBase,decay_type<PS_t>>::value>* = nullptr )
        {
            return solveImplCondense( ps, solution, rhs, name, kind, rebuild, pre, post,hana::integral_constant<int,decltype(hana::size( M_ps ))::value>() );
        }
    template <typename Solution_t, typename Rhs_t>
    typename Backend<double>::solve_return_type
    solveImpl( Solution_t& solution, Rhs_t const& rhs, std::string const& name, std::string const& kind,
               bool rebuild, pre_solve_type pre, post_solve_type post )
        {
            auto U = backend()->newBlockVector(_block=solution, _copy_values=false);
            tic();
            auto r1 = backend( _name=name, _kind=kind, _rebuild=rebuild,
                               _worldcomm=Environment::worldComm() )->solve( _matrix=this->matrixPtr(),
                                                                             _rhs=rhs.vectorPtr(),
                                                                             _solution=U,
                                                                             _pre=pre,
                                                                             _post=post
                                                                             );
            toc("blockform.monolithic",FLAGS_v>0);

            solution.localize(U);
            return r1;
        }
    template <typename PS_t, typename Solution_t, typename Rhs_t>
    typename Backend<double>::solve_return_type
    solveImplCondense( PS_t& ps, Solution_t& solution, Rhs_t const& rhs, std::string const& name, std::string const& kind,
                       bool rebuild, pre_solve_type pre, post_solve_type post , hana::integral_constant<int,1>,
                       std::enable_if_t<!std::is_base_of<ProductSpaceBase,decay_type<PS_t>>::value>* = nullptr )
        {
            return typename Backend<double>::solve_return_type{};
        }
    template <typename PS_t, typename Solution_t, typename Rhs_t> 
    typename Backend<double>::solve_return_type
    solveImplCondense( PS_t& ps, Solution_t& solution, Rhs_t const& rhs, std::string const& name, std::string const& kind,
                       bool rebuild, pre_solve_type pre, post_solve_type post , hana::integral_constant<int,2>,
                       std::enable_if_t<!std::is_base_of<ProductSpaceBase,decay_type<PS_t>>::value>* = nullptr )
        {
            return typename Backend<double>::solve_return_type{};
        }
    template <typename PS_t, typename Solution_t, typename Rhs_t>
    typename Backend<double>::solve_return_type
    solveImplCondense( PS_t& ps, Solution_t& solution, Rhs_t const& rhs, std::string const& name, std::string const& kind,
                       bool rebuild, pre_solve_type pre, post_solve_type post , hana::integral_constant<int,3>,
                       std::enable_if_t<!std::is_base_of<ProductSpaceBase,decay_type<PS_t>>::value>* = nullptr )
        {
            auto& e3 = solution(2_c);
            auto& e2 = solution(1_c);
            auto& e1 = solution(0_c);

            auto sc = this->matrixPtr()->sc();
#if 0
            this->matrixPtr()->printMatlab("A.m");
            rhs.vectorPtr()->printMatlab("F.m");
#endif

#if 0
            auto S = backend(_name="sc",_rebuild=true)->newMatrix( _test=e3.functionSpace(), _trial=e3.functionSpace(), _pattern=size_type(Pattern::EXTENDED)  );
            auto V = backend(_name="sc")->newVector( _test=e3.functionSpace() );
#else
            auto S = form2( _test=e3.functionSpace(), _trial=e3.functionSpace(), _pattern=size_type(Pattern::HDG)  );
            //MatSetOption ( dynamic_cast<MatrixPetsc<double>*>(S.matrixPtr().get())->mat(), MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_TRUE );
            auto V = form1( _test=e3.functionSpace() );

#endif

            //e3.printMatlab("phat.m");
            tic();
            this->syncLocalMatrix();
            sc->condense ( rhs.vectorPtr()->sc(), solution, S, V );
            toc("blockform.sc.condense", FLAGS_v>0);
            S.close();V.close();
            cout << " . Condensation done" << std::endl;
            tic();
            cout << " . starting Solve" << std::endl;
            auto r = backend(_name="sc",_rebuild=rebuild)->solve( _matrix=S.matrixPtr(), _rhs=V.vectorPtr(), _solution=e3);
            //auto r = backend(_name="sc",_rebuild=rebuild)->solve( _matrix=S, _rhs=V, _solution=e3);
            cout << " . Solve done" << std::endl;
            toc("blockform.sc.solve", FLAGS_v>0);

#if 0
            S.matrixPtr()->printMatlab("S.m");
            V.vectorPtr()->printMatlab("g.m");
            e3.printMatlab("phat1.m");
            e1.printMatlab("u.m");
            e2.printMatlab("p.m");
#endif
            tic();
            cout << " . starting local Solve" << std::endl;
            sc->localSolve ( rhs.vectorPtr()->sc(), solution);
            cout << " . local Solve done" << std::endl;
            toc("blockform.sc.localsolve",FLAGS_v>0);
#if 0
            e1.printMatlab("u1.m");
            e2.printMatlab("p1.m");
#endif

            return r;
        }

    //!
    //! solve using static condensation in the case of 2 trace spaces
    //!
    template <typename PS_t, typename Solution_t, typename Rhs_t>
    typename Backend<double>::solve_return_type
    solveImplCondense( PS_t& ps, Solution_t& solution, Rhs_t const& rhs, std::string const& name, std::string const& kind,
                       bool rebuild, pre_solve_type pre, post_solve_type post , hana::integral_constant<int,4>,
                       std::enable_if_t<!std::is_base_of<ProductSpaceBase,decay_type<PS_t>>::value>* = nullptr )
        {
            //auto& e4 = solution(3_c,0);
            auto& e3 = solution(2_c);
            auto& e2 = solution(1_c);
            auto& e1 = solution(0_c);

            auto sc = this->matrixPtr()->sc();
#if 0
            this->matrixPtr()->printMatlab("A.m");
            rhs.vectorPtr()->printMatlab("F.m");
#endif

            auto Th = product2( M_ps[3_c], M_ps[2_c] );
            auto S = blockform2(Th, Pattern::HDG);
            auto V = blockform1(Th);
            //MatSetOption ( dynamic_cast<MatrixPetsc<double>*>(S.matrixPtr().get())->mat(), MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_TRUE );
            auto U = Th.element();
            //e3.printMatlab("phat.m");
            tic();
            this->syncLocalMatrix();
            sc->condense ( rhs.vectorPtr()->sc(), solution, S, V );
            toc("blockform.sc.condense", FLAGS_v>0);
            S.close();V.close();
            cout << " . Condensation done" << std::endl;
            tic();
            cout << " . starting Solve" << std::endl;
            auto r = S.solve( _solution=U, _rhs=V, _name="sc",_rebuild=true );//, _condense=true );
            cout << " . Solve done" << std::endl;
            toc("blockform.sc.solve", FLAGS_v>0);

            solution(2_c)=U(0_c);
            for( int i = 0; i < Th[1_c]->numberOfSpaces(); ++i )
                solution(3_c,i)=U(1_c,i);
#if 0
            S.matrixPtr()->printMatlab("S.m");
            V.vectorPtr()->printMatlab("g.m");
            e3.printMatlab("phat1.m");
            e1.printMatlab("u.m");
            e2.printMatlab("p.m");
#endif
            tic();
            cout << " . starting local Solve" << std::endl;
            sc->setDim4( M_ps[3_c]->numberOfSpaces());
            sc->localSolve ( rhs.vectorPtr()->sc(), solution );
            cout << " . local Solve done" << std::endl;
            toc("blockform.sc.localsolve",FLAGS_v>0);
#if 0
            e1.printMatlab("u1.m");
            e2.printMatlab("p1.m"); 
#endif

            return r;
        }
    product_space_t M_ps;
    sparse_matrix_ptrtype M_matrix;
};

template<typename PS>
BlockBilinearForm<PS>
blockform2( PS&& ps, size_type pattern = Pattern::COUPLED )
{
    return BlockBilinearForm<PS>( std::forward<PS>(ps), pattern );
}
template<typename PS>
BlockBilinearForm<PS>
blockform2( PS&& ps, sparse_matrix_ptrtype m )
{
    return BlockBilinearForm<PS>( std::forward<PS>(ps), m );
}
/**
 * Handles linear form over a product of spaces
 */
template<typename PS>
class BlockLinearForm
{
public :
    using product_space_t = PS;

    BlockLinearForm() = default;
    BlockLinearForm(product_space_t&& ps )
        :
        M_ps(ps),
        M_vector(backend()->newBlockVector(_block=blockVector(M_ps), _copy_values=false))
        {}
    BlockLinearForm(product_space_t&& ps, vector_ptrtype v )
        :
        M_ps(ps),
        M_vector(v)
        {}
    BlockLinearForm( BlockLinearForm&& ) = default;
    BlockLinearForm( BlockLinearForm& ) = default;
    BlockLinearForm& operator=( BlockLinearForm && lf ) = default;
    BlockLinearForm& operator=( BlockLinearForm const& lf ) = default;

#if 0
    template<typename N1>
    decltype(auto) operator()( N1 n1 )
        {
            cout << "filling out vector block (" << n1 << ")\n";
            return form1(_test=M_ps[n1],_vector=M_vector, _rowstart=int(n1) );
        }
#endif
    template<typename N1>
    decltype(auto) operator()( N1 n1, int s = 0 )
        {
            int n = 0;
            auto&& spaces=hana::if_( hana::bool_<Feel::is_shared_ptr_v<PS>>{},
                                     []( auto&& x ) { return *x; },
                                     []( auto&& x ) { return x; } )(M_ps);
            auto space = hana::at( spaces, n1 );

            return hana::eval_if(std::is_base_of<ProductSpaceBase,decay_type<decltype(space)>>{},
                                 [&] (auto _) {
                                     cout << "filling out dyn vector block (" << int(n1) + s  << ")\n";
                                     return form1(_test=(*_(space))[s],_vector=M_vector->block(int(n1)+s), _rowstart=int(n1)+s );
                                 },
                                 [&] (auto _){
                                     cout << "filling out vector block (" << n1  << ")\n";
                                     return form1(_test=_(space),_vector=M_vector->block(int(n1)), _rowstart=int(n1) );
                                 });
        }

    decltype(auto) operator()( int n1 )
        {
            cout << "filling out vector block (" << n1 << ")\n";
            return form1(_test=M_ps[n1],_vector=M_vector->block(int(n1)), _rowstart=int(n1) );
        }
    void close()
        {
            M_vector->close();
        }
    product_space_t functionSpace() const { return M_ps; }

    vector_ptrtype vectorPtr() const { return M_vector; }
    vector_ptrtype vectorPtr() { return M_vector; }

    /**
     * set linear form to 0
     */
    void zero() { M_vector->zero(); }

    BlockLinearForm& operator+=( BlockLinearForm const& l )
        {
            if ( this == &l )
            {
                M_vector->scale( 2. );
                return *this;
            }

            *M_vector += *l.M_vector;

            return *this;
        }
    product_space_t M_ps;
    vector_ptrtype M_vector;
};

template<typename PS>
BlockLinearForm<PS>
blockform1( PS&& ps )
{
    return BlockLinearForm<PS>( std::forward<PS>(ps) );
}
template<typename PS>
BlockLinearForm<PS>
blockform1( PS&& ps, vector_ptrtype v )
{
    return BlockLinearForm<PS>( std::forward<PS>(ps), v );
}


}
#endif