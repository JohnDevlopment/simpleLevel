// empty tile
void Tile000(int* a, float* b, const SDL_Rect& c, const Point<int>& d, const ENTRY_POINT e) {
	switch (e) {
	  default: break;
	  
	  case ENTRY_PLAYER_DOWN:
	  case ENTRY_SPRITE_DOWN:
	  	CurrentSprite->UnsetColl(M_COLL_DOWN);
	  	break;
	  
	  case ENTRY_PLAYER_RIGHT:
	  case ENTRY_SPRITE_RIGHT:
	  	CurrentSprite->UnsetColl(M_COLL_RIGHT);
	  	break;
	  
	  case ENTRY_PLAYER_LEFT:
	  case ENTRY_SPRITE_LEFT:
	  	CurrentSprite->UnsetColl(M_COLL_LEFT);
	  	break;
	  
	  case ENTRY_PLAYER_UP:
	  case ENTRY_SPRITE_UP:
	  	CurrentSprite->UnsetColl(M_COLL_UP);
	  	break;
	}
}

// solid tile: pushes the sprite out from all sides
void SolidTile(int* iXYWH, float* fDtCGSpd, const SDL_Rect& hitbox, const Point<int>& collpoint, const ENTRY_POINT entry) {
	int diff;
	
	switch (entry) {
	  default: break;
	  
	  case ENTRY_PLAYER_DOWN:
	  case ENTRY_SPRITE_DOWN:
	  	{
	  	  // difference between the collision point and the top of the tile
	  	  diff = collpoint.y() - atile.y();
	  	  
	  	  // push the sprite upward by the difference between it and the tile
	  	  iXYWH[1] -= diff;
	  	  
	  	  // set the "floor collision" flag for the sprite being processed
	  	  CurrentSprite->SetColl(M_COLL_DOWN);
	  	  break;
	  	}
	  
	  case ENTRY_PLAYER_UP:
	  	{
	  	  // difference between the bottom of the tile and the collision point
	  	  diff = (atile.y() + TILE_HEIGHT) - collpoint.y();
	  	  
	  	  // push the sprite downward
	  	  iXYWH[1] += diff;
	  	  
	  	  // zero out the current Y speed
	  	  fDtCGSpd[3] = 0;
	  	  
	  	  // change the goal Y speed
	  	  fDtCGSpd[5] = 3;
	  	  
	  	  // set the "celing collision" flag for the sprite being processed
	  	  CurrentSprite->SetColl(M_COLL_UP);
	  	  break;
	  	}
	  
	  case ENTRY_PLAYER_RIGHT:
	  case ENTRY_SPRITE_RIGHT:
	  	{
	  	  // pull sprite back by the differenc between collision point and left edge of tile
	  	  diff = collpoint.x() - atile.x();
	  	  iXYWH[0] -= diff;
	  	  
	  	  // set the flag stating the sprite was blocked on its right side
	  	  CurrentSprite->SetColl(M_COLL_RIGHT);
	  	  break;
	  	}
	  
	  case ENTRY_PLAYER_LEFT:
	  case ENTRY_SPRITE_LEFT:
	  	{
	  	  // push sprite right by the difference between right edge of tile and collision point
	  	  diff = (atile.x() + TILE_WIDTH) - collpoint.x();
	  	  iXYWH[0] += diff;
	  	  
	  	  // set the flag stating the sprite was blocked on its left side
	  	  CurrentSprite->SetColl(M_COLL_LEFT);
	  	  break;
	  	}
	} // end switch
}

// ledge
void Ledge(int* iXYWH, float* fDtCGSpd, const SDL_Rect& hitbox, const Point<int>& collpoint, const ENTRY_POINT entry) {
	if (entry == ENTRY_PLAYER_DOWN || entry == ENTRY_SPRITE_DOWN) {
	  iXYWH[1] -= collpoint.y() - atile.y(); // pull sprite
	  CurrentSprite->SetColl(M_COLL_DOWN); // set collision flag
	}
}

// tile that blocks the sprite from going up through it
void OneWayDown(int* iXYWH, float* fDtCGSpd, const SDL_Rect& hitbox, const Point<int>& collpoint, const ENTRY_POINT entry) {
	// negative Y speed
	if (fDtCGSpd[3] < 0) {
	  // sprite hits bottom of tile
	  if (entry == ENTRY_PLAYER_UP || entry == ENTRY_SPRITE_UP) {
	  	// difference between the bottom of the sprite and collision point
	  	int diff = (atile.y() + TILE_HEIGHT) - collpoint.y();
	  	
	  	// push the sprite downward
	  	iXYWH[1] += diff;
	  	
	  	// zero out the current Y speed
	  	fDtCGSpd[3] = 0;
	  	
	  	// change the current goal speed
	  	fDtCGSpd[5] = 3;
	  	
	  	// collision flag
	  	CurrentSprite->SetColl(M_COLL_UP);
	  }
	}
}

void Slope(int* iXYWH, float* fDtCGSpd, const SDL_Rect& hitbox, const Point<int>& collpoint, const ENTRY_POINT entry) {
	constexpr int _bits = cinvert(0xff000000);
	
	// index to a table of height masks for the slope
	const short int* iOffsets = nullptr;
	{
	  int iSlpID = (iXYWH[0] & 0xff000000) >> 24;
	  iXYWH[0] &= _bits;
	  iOffsets = _slope_tables[iSlpID];
	}
	Point<int> copy_collpoint = collpoint;
	
	switch (entry) {
	  default: break;
	  
	  // up
	  case ENTRY_PLAYER_UP:
	  case ENTRY_SPRITE_UP:
	  	return;
	  	break;
	  
	  // left wall
	  case ENTRY_PLAYER_LEFT:
	  case ENTRY_SPRITE_LEFT:
	  	{
	  	  Point<int>& _coll = const_cast<Point<int>&>(collpoint);
	  	  _coll[0] += hitbox.w / 2;
	  	  _coll[1] += hitbox.h / 2;
	  	}
	  	break;
	  
	  // right wall
	  case ENTRY_PLAYER_RIGHT:
	  case ENTRY_SPRITE_RIGHT:
	  	copy_collpoint[0] -= hitbox.w / 2;
	  	copy_collpoint[1] += hitbox.h / 2;
	  	break;
	}
	
	// where collision point is relative to the tile
	Point<int> rel = copy_collpoint - atile;
	
	// point X is within the bounds of tile
	if (rel.x() >= 0 && rel.x() < TILE_WIDTH) {
	  // if rely already more than bottom of tile
	  if ( rel.y() > 31 ) {
	  	fDtCGSpd[7] = 0; // fractional Y position
	  	fDtCGSpd[3] = 0; // current Y speed
	  	
	  	const int iDiff = rel.y() - 30; // same as iDiff = rel.y() - 31 + 1
	  	iXYWH[1] -= iDiff;
	  	rel[1] -= iDiff; // relative coordinate (to the tile)
	  }
	  
	  /* If the Y speed is >= the difference between the tile bottom and
	     the point (relatively speaking), then reduce the Y speed (current and goal)
	     by that difference, further subtracted by 1. In theory, this should ensure the
	     sprite always hits the slope even if they move faster than the tile can handle. */
	  else if ( fDtCGSpd[3] >= (31 - rel.y()) ) {
	  	// how many bytes into the Object class before reaching the gravity delta
	  	constexpr size_t szIdx = sizeof(float) * 8 + sizeof(SDL_Rect) + sizeof(CollisionFunction);
	  	
	  	// pointer to the gravity delta, located at byte <szIdx>
	  	float* fpGravDt;
	  	addr_from_blk(fpGravDt, fDtCGSpd, szIdx);
	  	
	  	fDtCGSpd[7] = 0; // fractional Y position
	  	fDtCGSpd[3] = fDtCGSpd[5] = static_cast<float>(31 - rel.y() - 1) - *fpGravDt;
	  }
	  
	  
	  
	  // point Y is underneath the slope
	  if ( rel.y() > iOffsets[rel.x()] ) {
	  	iXYWH[1] -= rel.y() - iOffsets[rel.x()];
	  	fDtCGSpd[3] = 0;
	  	CurrentSprite->SetColl(M_COLL_DOWN); // set collision flag
	  }
//	  else {
//	  	CurrentSprite->UnsetColl(M_COLL_DOWN); // reset collision flag
//	  }
	}
}
