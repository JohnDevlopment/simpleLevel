static int EmptyTile(SDL_Rect* loc, SDL_Rect* col, const ENTRY_POINT entry) {
	
	if (entry == ENTRY_PLAYER_DOWN)
	  CurrentSprite->UnsetColl(M_COLL_DOWN);
	
return 0;
}

static int SlopeJoint(SDL_Rect* loc, SDL_Rect* col, const ENTRY_POINT entry) {
	loc->y -= 2;
	col->y -= 2;
	
return 0;
}

static int Ledge(SDL_Rect* loc, SDL_Rect* col, const ENTRY_POINT entry) {
	if (entry == ENTRY_PLAYER_DOWN) {
	  loc->y -= 1;
	  col->y -= 1;
	  return TILEFLAG_SOLID;
	}
	
return 0;
}
