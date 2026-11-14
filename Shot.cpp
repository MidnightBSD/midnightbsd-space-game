/*
 *  Shot.cpp
 *  Allenroids
 *
 *  Created by Allen Smith on Sat Jun 19 2004.
 *  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
 *
 */

#include "Shot.h"
#include "Renderer.h"

void Shot::drawCore(float x, float y) const {
	Renderer::current()->filledCircle(x, y, radius, 0.0F,
		{color[0] / 255.0F, color[1] / 255.0F, color[2] / 255.0F, 1.0F}, 12);
}
