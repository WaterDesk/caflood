#ifndef _CA_DECOMPOSITION_HPP_
#define _CA_DECOMPOSITION_HPP_

//! \file Decomposition.hpp
//! Contains the various methods/structures that perform domain and
//! data decomposition.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2012-07


#include<vector>
#include<list>

namespace CA {

    // ------------- Methods ----------- //

    //! Perform a domain decomposition and data partitioning using an
    //! efficiency/splitting algorithm similar to the one implemented in
    //! "An Integrated Decomposition and Partitioning Approach for
    //! Irregular Block-Structured Applications, Rantakokko J."

    //! The algorithm split the box with low efficiency recursevly by
    //! finding the line with least active points in a window of
    //! lines. The orientation of the split is chosen by seclection
    //! which one is larger between the width/2 and the height. The
    //! width is divided by two in order to favour height splitting.

    //! \attention This function uses a single box to indetify the area
    //! to decomposed. The data in this box is retrieved to the cell
    //! buffer into a temporary buffer. Thus check that the size of the
    //! box is not too big depending on the amount of memory available.

    //!\param[in]  box      Identifies the region of the grid to decomposed
    //!\param[in]  cellbuff The buffer to analyse.
    //!\param[in]  nodata   The value that identifies the non-domain cells.
    //!\param[in]  thrd     The efficiency threshould (number of domain points/ number of points).
    //!\param[in]  nlmin    Define the minimum nuber of lines in a side of the box.
    //!\param[out] boxl     The boxlist with the decomposed domain. Any previous
    //!                     values in the list are removed.

    template<typename T, typename CB>
    void splitBlockDecomposition(const Box& box, CB& cellbuff, T nodata, Real thrd, Unsigned nlmin,
        BoxList& boxl)
    {
        // List of temporary boxes.
        std::list<Box> list_boxes;

        // The iterator with the box that need to be processed.
        std::list<Box>::iterator    ib;

        // Create the temporary buffer where the retrieved values from the
        // CellBuffer of the original box  are stored and analysed.
        std::vector<T> buff(box.w()*box.h());

        // Retrieve the data from the CellBUff into the temporary buffer.
        cellbuff.retrieveData(box, &buff[0], box.w(), box.h());

        // Add the first box into the list of temporary boxes.
        list_boxes.push_back(box);

        // Select the first box to be analysed.
        ib = list_boxes.begin();

        // Loop until there are boxes to analyse
        while (ib != list_boxes.end())
        {
            // The temporary box to analyse is retrieved.
            Box tb = (*ib);

            // Select which direction to split:
            // dir = 0 -> split width.  lines <-> x and points <-> y
            // di  = 1 -> split height  lines <-> y and points <-> x

            int dir;
            if (tb.h() > tb.w() / 2 && tb.h() >= nlmin * 2)
                dir = 1;
            else
                dir = 0;

            // Compute the efficiency of the box for each line (x or y depends
            // on dir), i.e. the number of domain points/ the numbers of points.

            // Find the number of total lines of the direction to split.
            Unsigned nlines = (dir == 0) ? tb.w() : tb.h();

            // Find the number of points a line.
            Unsigned npoints = (dir == 0) ? tb.h() : tb.w();

            // Total about of domain points in the box.
            Unsigned ndp = 0;

            // Create a temporary array which contains the number of domain
            // points per line.
            std::vector<Unsigned> ndpl(nlines);

            // Now define the half point and the window of lines to analyse,
            // i.e. where the possible split could happen.
            Unsigned half = nlines / 2;
            Unsigned wb = half - 2;
            Unsigned we = half + 2;

            /*Check that wb and we do not make a too small box*/
            if (wb < nlmin)
                wb = half;
            if (nlines - we < nlmin)
                we = half;

            // The line to split. The default value is half.
            Unsigned ls = half;

            // Loop throug each line and compute the number of domains and
            // total points per line. Compute as well the total number of
            // domain points in the box.
            for (Unsigned l = 0; l < nlines; l++)
            {
                // Loop through each point in the line and check if is a domain
                // or not value.  
                for (Unsigned p = 0; p < npoints; p++)
                {
                    //Retrieve the value of the buffer.
                    Unsigned x = (tb.x() - box.x()) + ((dir == 0) ? l : p);
                    Unsigned y = (tb.y() - box.y()) + ((dir == 0) ? p : l);

                    T value = buff[y*box.w() + x];

                    // Check if is part of the domain.
                    if (value != nodata)
                    {
                        ndp++;
                        ndpl[l] += 1;
                    }
                }

                // If the number of domain point of this line is zero and the
                // previous one is not zero, set this line to be the split
                // one.
                if ((l > nlmin && l < nlines - nlmin - 1) && ndpl[l] == 0 && ndpl[l - 1] != 0)
                    ls = l;

                // If the number of domain point of this line is not zero and
                // the previous one is zero, set the previous one to be the
                // split one.
                if ((l > nlmin + 1 && l < nlines - nlmin) && ndpl[l - 1] == 0 && ndpl[l] != 0)
                    ls = l - 1;
            }

            // Update the efficiency
            (*ib).setE((double)ndp / (double)(tb.w()*tb.h()));

            // If this box efficiency is higher that the threshould process
            // the next box in the list. Do the same also if the efficiency is zero.
            if ((*ib).e() > thrd || (*ib).e() == 0.0)
            {
                ib++;
                continue;
            }

            // If the number of lines is smaller than the double of the
            // minimum do not split and go to the next box to analyse.
            if (nlines < 2 * nlmin)
            {
                ib++;
                continue;
            }

            // Loop through the window and find the line with less point if
            // we didn't find one with zero points before.
            for (Unsigned l = wb; l <= we; l++)
            {
                if (ndpl[l] < ndpl[ls])
                    ls = l;
            }

            // Create two new boxes equal to the original one.
            Box A(tb);
            Box B(tb);

            // Split the two new boxes.
            if (dir == 0)
            {
                A.setW(ls);
                B.setW(nlines - ls);
                B.setX(A.x() + ls);
            }
            else
            {
                A.setH(ls);
                B.setH(nlines - ls);
                B.setY(A.y() + ls);
            }

            // Insert the two new boxes. Insert B first and then A since the
            // method insert before the given iterator.
            std::list<Box>::iterator    ibn = list_boxes.insert(list_boxes.insert(ib, B), A);

            // Erase the box just processed.
            list_boxes.erase(ib);

            // Set the next box to be the next box processed.
            ib = ibn;

            // Now compute the efficiency of box A.
            Unsigned ndpA = 0;

            for (Unsigned l = 0; l < ls; l++)
                ndpA += ndpl[l];

            // If the efficiency of A is ower than threshould, make the next
            // box processed to be B.
            (*ib).setE((double)ndpA / (double)(A.w()*A.h()));
            if ((*ib).e() > thrd || (*ib).e() == 0.0)
            {
                ib++;
                Unsigned ndpB = 0;

                // However check efficiency of B.
                for (Unsigned l = ls; l < nlines; l++)
                    ndpB += ndpl[l];

                // If the efficiency of B is ower than threshould, process the next box.
                (*ib).setE((double)ndpB / (double)(B.w()*B.h()));
                if ((*ib).e() > thrd || (*ib).e() == 0.0)
                {
                    ib++;
                }
            }

        } // End while

        // Clar the output box.
        boxl.clear();

        // Copy from the temporary box to the output box.
        for (ib = list_boxes.begin(); ib != list_boxes.end(); ib++)
        {
            boxl.add((*ib));
        }

    } // End method

} // Namespace CA

#endif // _CA_DECOMPOSITION_HPP_
