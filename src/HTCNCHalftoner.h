/******************************************************************************
* CNC Halftone Wizard
* Copyright (C) 2011 Paul Kerchen
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


#ifndef HTCNCHALFTONER_H
#define HTCNCHALFTONER_H

#include <QString>

// Forward decls
class QPixmap;
class QImage;

namespace HTCNC
{

	/*@brief Converts arbitrary images to halftone images as well as CNC instructions.
	 **/
	class Halftoner
	{
		public:
			// This is a diagram showing which measurements
			// correspond to which values in the CNCParameters struct.
			//
			//        ||| <-- Cutting tool
			//        |||
			//   _____|||_____    __________
			//  \             /       ^
			// | \           / |      |
			// |  \         /  |      |
			// |   \       /   |     Full
			// |    \     /    |     Tool 
			// |     \   /     |     Depth 
			// |      \ /      |      |
			// |       .       |      v
			// |               |   ----------
			// |     Full      |
			// |<----Tool----->|
			//       Width
			//
			//
			typedef struct
			{
				int			m_step;						/// The number of pixels to skip between dots
				double	m_fullToolDepth;	/// Depth that can be cut by tool
				double	m_fullToolWidth;	/// Width of tool at full tool depth
				double	m_maxCutPercent;	/// Percentage used to compute max cut depth/diameter
				double	m_minDotGap;			/// Minimum gap between dots
				double	m_fastZ;					/// Z depth where tool can be moved quickly
			} CNCParameters;


			/**
			 * @brief Constructs a Halftoner object and performs all the output calculations.
			 * @param src The source pixmap to be halftoned.
			 * @param dest The destination image that will recieve the preview image
			 * of the halftoned version of src.
			 * @param scale The scale factor for the preview image.  Should be >= 1.
			 * @param generateGCode If true, g code is generated (g code generation
			 * can be time-consuming and may not be needed if all the user is doing
			 * is trying out different parameters to see what they look like in the
			 * preview).
			 * @param params The parameters that control the generated g-code.
			 **/
			Halftoner( const QPixmap& src, QImage& dest, int scale, bool generateGCode, const CNCParameters& params );


			virtual ~Halftoner()
			{
			}

			/// Returns the number of cuts (Z up/down movements) needed to make the
			/// image computed in the constructor.
			int getCutCount() const
			{
				return m_cutCount;
			}

			/// Returns the g code needed to do the actual cutter movement (no
			/// pre/post-amble).
			QString getGCode() const 
			{ 
				return m_gCode; 
			}

		protected:
			/// The number of dots that will need to be cut.
			int	m_cutCount;
			/// The g code needed to cut the dots (does not include preamble or
			//postamble--just the "G0X...Y... G1Z..." needed to move the cutter
			//around, up and down).
			QString	m_gCode;
	};

}	// namespace HTCNC


#endif

