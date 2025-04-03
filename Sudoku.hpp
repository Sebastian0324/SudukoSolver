#pragma once
#include <iostream>

/// @brief 
/// @param n int32
/// @param length the number of bitts shown
/// @return the binary reprisentation off the number
std::string intToBinary(int n, int length = 32) {
    std::string output = "";

    while (length > 0)
    {
        output = ((n & 0b1)? '1' : '0') + output;

        n >>= 1;
        length--;
    }

    return output;
}

class Sudoku
{
protected:
    /// @brief number, flags == numbers this can be, the chunk this tile is in
    struct Tile
    {
        unsigned int flags = 0b111111111;
        int number = 0;
        
        int relatedChunk;
    };

    /// @brief a set of 9 tiles
    struct TileSet
    {
        /// @brief numbers that can go into this set of tiles
        unsigned int flags = 0b111111111;
        Tile* set[9];

        /// @brief checkse each tile in the set and see if the number can go into it, if so return it
        /// @param set the tileSet that is checked
        /// @param numb the number that is checked
        /// @return the tiles that the numbers can go into, flags is used to identify the number of tiles that fitt the creiteria
        TileSet* possibleTiles(Tile* set[9], int numb) {
            TileSet* tiles = new TileSet;
            tiles->flags = 0;

            for (int i = 0; i < 9; i++)
            {
                if ((set[i]->flags & (1 << (numb - 1))) == 0)
                    continue;

                // tiles->set[tiles->flags]->flags = set[i]->flags;
                // tiles->set[tiles->flags]->number = set[i]->number;
                // tiles->set[tiles->flags]->relatedChunk = set[i]->relatedChunk;
                    tiles->set[tiles->flags] = set[i];
                tiles->flags++;
            }
            
            return tiles;
        }
    };
private:

    TileSet* row[9];
    TileSet* colum[9];
    TileSet* chunk[9];

    /// @brief inserts a specific number in all forced places
    /// @param numb the number that is checked
    /// @return true if a number was inserted
    bool fillN(int numb) {
        bool change = 0;
        TileSet* temp = nullptr;
        for (int i = 0; i < 9; i++)
        {
            temp = row[i]->possibleTiles(row[i]->set, numb);
            if (temp->flags == 1)
            {
                // if only this number can go into this tile, insert it then exit the loop
                for (int j = 0; j < 9; j++)
                {
                    if ((row[i]->set[j]->flags & (1 << (numb - 1))) == 0)
                        continue;
                    
                    insert(numb, j, i);
                    change = true;
                    break;
                }
            }
            delete temp;
            temp = colum[i]->possibleTiles(colum[i]->set, numb);
            if (temp->flags == 1)
            {
                for (int j = 0; j < 9; j++)
                {
                    if ((colum[i]->set[j]->flags & (1 << (numb - 1))) == 0)
                        continue;
                    
                    insert(numb, i, j);
                    change = true;
                    break;
                }
            }
            delete temp;
            temp = chunk[i]->possibleTiles(chunk[i]->set, numb);
            if (temp->flags == 1)
            {
                for (int j = 0; j < 9; j++)
                {
                    if ((chunk[i]->set[j]->flags & (1 << (numb - 1))) == 0)
                        continue;
                    
                    insert(numb, (j / 3) *  9 + (j % 3) + (i / 3) * 27 + ((3 * i) % 9));
                    change = true;
                    break;
                }
            }
            delete temp;
        }

        return change;
    }

    void rowLogic(TileSet* tile_set[])
    {
        // flags related to numb in tile_set[i-i+3]
        int possibilites;

        for (int numb = 0; numb < 9; numb++)
        {
            for (int i = 0; i < 3; i++)
            {
                possibilites = 0;

                // checks if the number into tow or more subsets else continue
                for (int j = 0; j < 3; j++)
                {
                    if ((tile_set[j + (i * 3)]->flags & (1 << numb)) != 0)
                        possibilites++;
                }
                if (possibilites <= 1)
                    continue;

                possibilites = 0;

                // checks if one chunk only have one tile_set the number can go into
                for (int j = 0; j < 3; j++)
                {
                    if ((chunk[tile_set[i * 3]->set[j * 3]->relatedChunk]->flags & (1 << numb)) == 0)
                        continue;
                    
                    for (int k = 0; k < 3; k++) 
                    {
                        // if the number can go into the tile_set of the chunk add 1 to the end of possibilites
                        possibilites = (possibilites << 1) + ((canFitInRowOfChunk(tile_set[i * 3]->set[j * 3]->relatedChunk, (1 << numb), k, tile_set[0]->set[3]->relatedChunk - 1)));
                    }

                    // if this chunk can only have the number in one tile_set then
                    // no other chunk(in that tile_set) can have that number in that tile_set
                    for (int k = 0; k < 3; k++)
                    {
                        if (((possibilites & 0b111) & (~(1 << k))) == 0 && (possibilites & 0b111) != 0)
                            removeFlags(tile_set[(i * 3) + (2 - k)]->set, (1 << numb), ((j * 3 + 3) % 9), 6);
                    }
                }

                for (int j = 0; j < 3; j++)
                {
                    // if the number can go into all rows or no row -> continue 

                    if (((possibilites >> (j * 3)) & 0b111) == 0b111 || ((possibilites >> (j * 3)) & 0b111) == 0b000
                     || ((possibilites >> (j * 3)) & 0b111) != ((possibilites >> ((j + 1) * 3 % 9)) & 0b111) || ((possibilites >> ((j + 2) * 3 % 9)) & 0b111) == 000)
                        continue;
                    // if the number cant go into all chunks of the row/colom -> continue
                    for (int k = 0; k < 3; k++)
                    {
                        if (((possibilites >> k) & 0b001001001) != 0b001001001)
                            continue;

                        removeFlags(tile_set[(i * 3) + ((2 - k) % 3)]->set, (1 << numb), (((9 - (3 * j)) + 9) % 9), 3);
                    }
                }
            }
        }
    }

    void setLogic(TileSet* tile_set[], TileSet matchingSets[6], int i = 0, int numb = 1) {
        //  ---------------------------------------------------------------------------------------
        //  TO DO
        //  Find Out What This Code Does And Fix It!!!
        //  ---------------------------------------------------------------------------------------
        TileSet* poss;
        int nrOfMatches = 0;
        for (i = 0; i < 9; i++)
        {
            for (numb; numb <= 9; numb++)
            {
                poss = tile_set[i]->possibleTiles(tile_set[i]->set, numb);

                if (poss->flags > 3  || poss->flags <= 1 || poss->flags > matchingSets[0].flags && matchingSets[0].flags != 0b111111111)
                {
                    delete poss;
                    continue;
                }

                nrOfMatches = 0;

                for (int j = 0; j < poss->flags && matchingSets[j].flags != 0b111111111; j++)
                {
                    for (int k = 0; k < matchingSets[0].flags && j < poss->flags; k++)
                    {
                        if (poss->set[j] == matchingSets[0].set[k])
                        {
                            nrOfMatches++;
                            j++;
                        }
                    }
                }
                
                //  ---------------------------------------------------------------------------------------
                //  TO DO

                //  number is unknown
                //  index is unknown
                //  if statment is broken
                //  ---------------------------------------------------------------------------------------

                // if (nrOfMatches < poss.flags)
                //     continue;
                
                int j = 0;
                while (matchingSets[j].flags != 0b111111111)
                {
                    j++;
                }
                
                matchingSets[j] = *poss;
                matchingSets[j].set[0]->number = numb;

                if (j == matchingSets[0].flags)
                {
                    for (int k = 0; k < matchingSets[0].flags; k++)
                    {
                        // tile_set[i]->set[0]->flags = (tile_set[i]->set[0]->flags & (1 << matchingSets[k].set[0]->number));
                        for (int test = 0; test < matchingSets[k].flags; test++)
                        {
                            std::cout << numb << " | " << intToBinary(matchingSets[k].set[test]->flags, 9) << " | " << matchingSets[k].set[test]->number << " | " << matchingSets[k].set[test]->relatedChunk << std::endl;
                        }
                    }
                    
                }
                else if (numb + 1 < 9)
                    setLogic(tile_set, matchingSets, i, numb + 1);

                delete poss;
            }
        }
    }

public:
    Tile* gameBoard[81];

    TileSet matchingSets[6];

    Sudoku() {
        //pre sets gameBoard so that TileSet has something to point to
        for (int i = 0; i < 81; i++)
        {
            gameBoard[i] = new Tile;
        }
        
        //sets the pointers of all TileSets
        for (int i = 0; i < 9; i++)
        {
            row[i] = new TileSet;
            colum[i] = new TileSet;
            chunk[i] = new TileSet;

            for (int j = 0; j < 9; j++)
            {
                //link the sets to the tiles
                row[i]->set[j] = gameBoard[i * 9 + j];
                colum[i]->set[j] = gameBoard[j * 9 + i];
                chunk[i]->set[j] = gameBoard[(j / 3) *  9 + (j % 3) + (i / 3) * 27 + ((3 * i) % 9)];
                //link the tiles to the sets(it is not needed to link row or colum)
                gameBoard[(j / 3) *  9 + (j % 3) + (i / 3) * 27 + ((3 * i) % 9)]->relatedChunk = i;
            }
        }

        print();
    }
    
    /// @brief resets all numbers to 0 and all flags to 1
    void Restart() {
        for (Tile* tile : gameBoard)
        {
            tile->number = 0;
            tile->flags = 0b111111111;
        }
        
        for (int i = 0; i < 9; i++)
        {
            row[i]->flags = 0b111111111;
            colum[i]->flags = 0b111111111;
            chunk[i]->flags = 0b111111111;
        }

        print();
    }

    void solve() {
        int unknownTiles = 0;
        for (int i = 1; i <= 9; i++)
        {
            if( fillN(i) )
                i = 0;
        }
        std::cout << "/*-----End of recurtion-----*/" << std::endl;
        for (int i = 0; i < 81; i++)
        {
            if (gameBoard[i]->flags == 0)
            {
                if (gameBoard[i]->number != 0)
                    continue;

                std::cout << "This game has no awnser" << std::endl;
                return;
            }

            for (int j = 0; j < 9; j++)
            {
                if (gameBoard[i]->flags == (1 << j))
                {
                    insert(j + 1, i);
                    break;
                }
            }
            if (gameBoard[i]->flags != 0)
                unknownTiles++;
        }
        std::cout << "There is " << unknownTiles << " unknown tiles" << std::endl;
    }

    void logic() 
    {
        /* remove flags */
        rowLogic(row);
        rowLogic(colum);

        setLogic(row, matchingSets);
    }

    /// @brief outputs all tiles index: number | flags
    void debug() {
        for (int i = 0; i < 81; i++)
        {
            std::cout << i << ": " << gameBoard[i]->number << " | " << intToBinary( gameBoard[i]->flags, 9 ) << std::endl;
        }
    }
    /// @brief outputs all relavent information about selected tile
    /// @param x distence from the left side
    /// @param y distence from the top
    /// @param numb show the number of posibale positions for this number
    void debug(int x, int y, int numb = 1) {
        TileSet* possRow = row[y]->possibleTiles(row[y]->set, numb);
        TileSet* possColum = colum[x]->possibleTiles(colum[x]->set, numb);
        TileSet* possChunk = chunk[row[x]->set[y]->relatedChunk]->possibleTiles(chunk[row[y]->set[x]->relatedChunk]->set, numb);

        std::cout << "Flags:" << std::endl;
        std::cout << "tile->flags: " << row[y]->set[x]->flags << " ; " << intToBinary(row[y]->set[x]->flags, 9) << " ; number: " << row[y]->set[x]->number << " ; index: " << y * 9 + x << std::endl;
        std::cout << "row->flags: " << row[y]->flags << " ; " << y << " ; "<< possRow->flags << std::endl;
        std::cout << "colum->flags: " << colum[x]->flags << " ; " << x << " ; "<< possColum->flags << std::endl;
        std::cout << "chunk->flags: " << chunk[row[y]->set[x]->relatedChunk]->flags << " ; " << row[y]->set[x]->relatedChunk << " ; "<< possChunk->flags << std::endl;

        delete possRow;
        delete possColum;
        delete possChunk;
    }

    /// @brief fix the inputs to the correct format then call the true funktion
    /// @param numb the number that is being inserted
    /// @param position the tiles position acording to gameBoard
    void insert(int numb, int position) {
        insert(numb, position % 9, position / 9);
    }
    /// @brief checks if the tile can have the number and if true inserts if and changes the related flags
    /// @param numb the number that the user wants to insert
    /// @param x distence from the left side
    /// @param y distence from the top
    void insert(int numb, int x, int y, bool printIfSuccess = true) {
        //chek if the tile is emty and can have the number in it
        if (gameBoard[x + (y * 9)]->number != 0 || (gameBoard[x + (y * 9)]->flags & (1 << (numb - 1))) == 0)
        {
            std::cout << "tile is taken or flag is set" << std::endl;
            return;
        }
        if (printIfSuccess)
            std::cout << "success" << std::endl;
        
        //add the numbert to the gameboard
        gameBoard[x + (y * 9)]->number = numb;

        //loop throgh and remove all relevant flags
        for (int i = 0; i < 9; i++)
        {
            row[y]->set[i]->flags &= ~(1 << (numb - 1));
            colum[x]->set[i]->flags &= ~(1 << (numb - 1));
            chunk[gameBoard[x + (y * 9)]->relatedChunk]->set[i]->flags &= ~(1 << (numb - 1));
        }
        // removes all flags from selected tile
        // then removes the flag for this number from all relavent tileSets
        gameBoard[x + (y * 9)]->flags = 0; 
        row[y]->flags &= ~(1 << (numb - 1));
        colum[x]->flags &= ~(1 << (numb - 1));
        chunk[gameBoard[x + (y * 9)]->relatedChunk]->flags &= ~(1 << (numb - 1));
    }

    /// @brief removes the selected flag from the selected run of tiles
    /// @param set the set that contain the tiles that the flag will be removed from
    /// @param flag the flag that will be removed
    /// @param start the starting index
    /// @param length the number of tiles that the flag will be removed from
    void removeFlags(Tile* set[], int flag, int start, int length) {
        while (length > 0)
        {
            set[start % 9]->flags &= ~flag;
            length--;
            start++;
        }
    }

    /// @brief 
    /// @param chunk_i the index for the relavent cunk
    /// @param flag the flag for the number that is checked
    /// @param row_or_cplum determens the row or colum that is checked
    /// @param check_colum 0 if it's checks rown or 2 if it's checks colums
    /// @return return true if the number can go into the row or colum that is checked inside the relavent chunk
    bool canFitInRowOfChunk(const int chunk_i, int flag, int row_or_colum, int check_colum) {
        for (int i = 0; i < 3; i++)
        {
            if (chunk[chunk_i]->set[row_or_colum * (3 - check_colum) + i * (check_colum + 1)]->flags & flag)
                return 1;
        }
        return 0;
    }

    /// @brief Prints a humen readebl vertion of the gameboard. if the number is a zero it prints a ' ' insted
    void print() {
        for (int i = 0; i < 81; i++)
        {
            std::cout << (char)(48 + ((gameBoard[i]->number)? gameBoard[i]->number : -16)) << ' ';
            if (!((i + 1) % 27) && i != 80)
            {
                std::cout << std::endl << "----------------------" << std::endl;
            }
            else if (!((i + 1) % 9))
            {
                std::cout << std::endl;
            }
            else if (!((i + 1) % 3))
            {
                std::cout << "| ";
            }
        }
    }
};