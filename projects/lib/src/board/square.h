/*
    This file is part of GGzero Chess.
    Copyright (C) 2008-2018 GGzero Chess authors

    GGzero Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GGzero Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GGzero Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SQUARE_H
#define SQUARE_H

#include <QString>

#pragma execution_character_set("utf-8")   // ��ʾ����

namespace Chess {

/*!
* \brief A generic chess square type consisting of a file and a rank.
*
* Square is mainly used as a middle-layer between the Board
* class (which uses integers for squares) and more generic, high-level
* classes like GenericMove.
*/
class LIB_EXPORT Square
{
	public:
		/*! Square color */
		//enum Color
		//{
		//	Light,	//!< Light-colored square.
		//	Dark,	//!< Dark-colored square.
		//	NoColor	//!< Color for invalid squares.
		//};

		/*! Creates a new square with invalid defaults. */
		Square();
		/*! Creates a new square from \a file and \a rank. */
		Square(int file, int rank);

		/*! Returns true if \a other is the same as this square. */
		bool operator==(const Square& other) const;
		/*! Returns true if \a other is different from this square. */
		bool operator!=(const Square& other) const;

		/*! Returns true if both file and rank have non-negative values. */
		bool isValid() const;

		/*! Zero-based file of the square. 0 is the 'a' file. */
		int file() const;
		/*! Zero-based rank of the square. 0 is white's first rank. */
		int rank() const;
		/*! Returns the color of the square. */
		//Color color() const;

		/*! Sets the file to \a file. */
		void setFile(int file);
		/*! Sets the rank to \a rank. */
		void setRank(int rank);

		QString as_qstring();

		//static int getSq(int f, int r);

	private:
		int m_file;
		int m_rank;
};

} // namespace Chess
#endif // SQUARE_H
