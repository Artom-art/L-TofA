#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

typedef std::vector<std::vector<int>> Matrix;

struct Point
{
	int row;
	int column;
};

Point GetPoint(Matrix &mat, int rows, int columns, int num)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if (mat[i][j] == num)
			{
				mat[i][j] = (mat[i][j] == INT_MIN) ? INT_MIN + 1 : mat[i][j];
				return {i, j};
			}
		}
	}
	return {0, 0};
}

void SetCraterNum(Matrix &island, int rows, int columns, char ch, int column, int row)
{
	int number = 0;
	if (island[row - 1][column] != 0 && island[row][column - 1] == 0)
	{
		number = island[row - 1][column];
	}
	else if (island[row - 1][column] == 0 && island[row][column - 1] != 0)
	{
		number = island[row][column - 1];
	}
	else if (island[row - 1][column] != 0 && island[row][column - 1] != 0)
	{
		int leftNum = island[row - 1][column];
		int topNum = island[row][column - 1];
		if (leftNum < topNum)
		{
			number = leftNum;
		}
		else
		{
			number = topNum;
			std::swap(leftNum, topNum);
		}
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				if (island[i][j] == topNum)
				{
					island[i][j] = leftNum;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				if (number < island[i][j])
				{
					number = island [i][j];
				}
			}
		}
		number += 1;
	}
	island[row][column] = number;
}

void PrintWay(std::ofstream &out, Matrix &distMat, int rows, int columns)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if (distMat[i][j] == 0)
			{
				out << "*";
			}
			else if (distMat[i][j] < 0)
			{
				out << "X";
			}
			else
			{
				out << ".";
			}
		}
		out << std::endl;
	}
}

void InitiateIsland(std::ifstream &input, Matrix &island, int rows, int columns)
{
	std::string line;
	int strAmount = 0;
	while (std::getline(input, line) && strAmount < rows)
	{
		size_t i = 0;
		size_t length = line.length();
		while (i < length && i < columns)
		{
			// Установка меток на острове.
			if (line[i] == '.')
			{
				island[strAmount][i] = 0;
			} 
			else
			{
				SetCraterNum(island, rows, columns, line[i], i, strAmount);
			}
			i += 1;
		}
		strAmount += 1;
	}
}

void ParametersSeparation(char *argv[], std::ifstream &input, std::ofstream &output)
{
	input.open(argv[1]);
	output.open(argv[2]);
}

Matrix InitiateVector(int rows, int columns, int value)
{
	std::vector<std::vector<int>> vector;
	for (int i = 0; i < rows; i++) 
	{
		std::vector<int> vectorRow;
		for (int j = 0; j < columns; j++) 
		{
			vectorRow.push_back(value);
		}
		vector.push_back(vectorRow);
	}
	return vector;
}

void FillDistanceFromTop(Matrix &distanceMat, Matrix const &island, int row, int columns)
{
	for (int i = 0; i < columns; i++)
	{
		if (row == 0 || i == 0)
		{
            distanceMat[row][i] = 1;
		}
		else
		{
			if (island[row][i] != 0)
			{
				distanceMat[row][i] = -island[row][i];
			}
			else
			{
				int a = distanceMat[row - 1][i];
				int b = distanceMat[row][i - 1];
				if (a < 0) {a = INT_MAX;}
				if (b < 0) {b = INT_MAX;}
				int c = (a < b) ? a: b;
				if (distanceMat[row][i] - 1 > c)
				{
					distanceMat[row][i] = c;
					if (distanceMat[row][i] != INT_MAX) {distanceMat[row][i]++;}
				}
			}
		}
	}
}

void FillDistanceFromDown(Matrix &distanceMat, int rows, int columns, int row)
{
	for (int i = columns - 1; i >= 0; i--)
	{
		if (row == rows - 1 || i == columns - 1)
		{
            distanceMat[row][i] = 1;
		}
		else
		{
			if (distanceMat[row][i] > 0)
			{
				int a = distanceMat[row + 1][i];
				int b = distanceMat[row][i + 1];
				if (a < 0) {a = INT_MAX;}
				if (b < 0) {b = INT_MAX;}
				int c = (a < b) ? a : b;
				if (distanceMat[row][i] - 1 > c)
				{
					distanceMat[row][i] = c;
					if (c != INT_MAX) {distanceMat[row][i]++;}
				}
			}
		}
	}
}

Matrix FillInDistances(Matrix const &island, int rows, int columns)
{
	Matrix distanceMat = InitiateVector(rows, columns, INT_MAX);
	for (int row = 0; row < rows; row++)
	{
		FillDistanceFromTop(distanceMat, island, row, columns);
		FillDistanceFromDown(distanceMat, rows, columns, row);
	}
	for (int row = rows - 1; row >= 0; row--)
	{
		FillDistanceFromDown(distanceMat, rows, columns, row);
		FillDistanceFromTop(distanceMat, island, row, columns);
	}
	return distanceMat;
}

void CheckWayToPoint(Matrix const& distanceMat, std::vector<Point>& way,
					int rows, int columns, int row, int column)
{
	int left, right, top, down = 0;
	while (row != 0 && column != 0 && row + 1 != rows && column + 1 != columns)
	{
		left = (distanceMat[row][column - 1] > 0) ? distanceMat[row][column - 1] : INT_MAX;
		right = (distanceMat[row][column + 1] > 0) ? distanceMat[row][column + 1] : INT_MAX;
		top = (distanceMat[row - 1][column] > 0) ? distanceMat[row - 1][column] : INT_MAX;
		down = (distanceMat[row + 1][column] > 0) ? distanceMat[row + 1][column] : INT_MAX;
		if (left < right && left < top && left < down) {column = column - 1;}
		if (down < left && down < top && down < right) {row = row + 1;}
		if (right < left && right < down && right < top) {column = column + 1;}
		if (top < left && top < down && top < right) {row = row - 1;}
	}
}

std::vector<Point> GetMinWay(Matrix &distanceMat, int rows, int columns)
{
	std::vector<Point> way;
	for (int row = 0; row < rows; row++)
	{
		for (int column = 0; column < columns; column++)
		{
			if (distanceMat[row][column] < 0)
			{
				CheckWayToPoint(distanceMat, way, rows, columns, row, column);
			}
		}
	}
	return way;
}

void SendStep(Matrix &mat, std::vector<int> &craterNums, int step, int i, int j, 
			  int rows, int columns, bool &added, int &length)
{
	if (i >= 0 && j >= 0 && i < rows && j < columns)
	{
		// Если (i, j) уже добавлено или непроходимо, то не обрабатываем 
		if (mat[i][j] == -1)
		{
			mat[i][j] = step;
			added = true;
		}
		if (mat[i][j] <= -2 && std::find(craterNums.begin(), craterNums.end(), mat[i][j]) == craterNums.end() && mat[i][j] != INT_MIN)
		{
			length += step - 1;
			craterNums.push_back(mat[i][j]);
			mat[i][j] = INT_MIN;
		}
	}
}

Matrix SendWave(Matrix const &distanceMat, int row, int column, int rows, int columns, int &length)
{
	int step = 0;
	Matrix mat = InitiateVector(rows, columns, 0);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if (distanceMat[i][j] > 0)
			{
				mat[i][j] = -1;
			}
			else
			{
				mat[i][j] = distanceMat[i][j] - 1;
			}
		}
	}
	mat[row][column] = 0;
	bool added = true;
	std::vector<int> craterNums;
	while (added)
	{
		added = false;
		step++;
		int i = (row - step > 0) ? row - step : 0;
		while (i < row + step && i < rows)
		{
			int j = (column - step > 0) ? column - step : 0;
			while (j < column + step && j < columns)
			{
				if (mat[i][j] == step - 1)
				{
					SendStep(mat, craterNums, step, i + 1, j, rows, columns, added, length);
					SendStep(mat, craterNums, step, i - 1, j, rows, columns, added, length);
					SendStep(mat, craterNums, step, i, j + 1, rows, columns, added, length);
					SendStep(mat, craterNums, step, i, j - 1, rows, columns, added, length);

					if (craterNums.size() >= 2)
					{
						return mat;
					}
				}
				j++;
			}
			i++;
		}
	}
	return mat;
}

int MinValueWithPoint(Matrix const &distanceMat, int row, int column, int rows, int columns)
{
	int left = (column > 0 && distanceMat[row][column - 1] > 0) ? distanceMat[row][column - 1] : INT_MAX;
	int right = (column < columns - 1 && distanceMat[row][column + 1] > 0) ? distanceMat[row][column + 1] : INT_MAX;
	int top = (row > 0 && distanceMat[row - 1][column] > 0) ? distanceMat[row - 1][column] : INT_MAX;
	int down = (row < rows - 1 && distanceMat[row + 1][column] > 0) ? distanceMat[row + 1][column] : INT_MAX;
	int minInHorizontal = (left < right) ? left : right;
	int minInVertical = (top < down) ? top : down;
	return (minInHorizontal < minInVertical) ? minInHorizontal : minInVertical;
}

int GetMaxElement(Matrix const &mat, int rows, int columns)
{
	int max = INT_MIN;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if (mat[i][j] > max)
			{
				max = mat[i][j];
			}
		}
	}
	return max;
}

Matrix FindPoint(Matrix const &distanceMat, int rows, int columns)
{
	Matrix newMat = InitiateVector(rows, columns, INT_MAX);
	Matrix prevMat = InitiateVector(rows, columns, INT_MAX);
	int newLength = INT_MAX;
	int prevLength = INT_MAX;
	for (int row = 1; row < rows - 1; row++)
	{
		for (int column = 1; column < columns - 1; column++)
		{
			newLength = 0;
			if (distanceMat[row][column] > 0 && distanceMat[row][column] != INT_MAX)
			{
				newMat = SendWave(distanceMat, row, column, rows, columns, newLength);
				newLength = newLength + MinValueWithPoint(distanceMat, row, column, rows, columns); 
				if (newLength < prevLength)
				{
					prevMat = newMat;
					prevLength = newLength;
				}
			}
        }
	}
	return prevMat;
}

int CountWayLength(Matrix const &islandWithPoint, Matrix& distanceMat, int rows, int columns)
{
	int length = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int l = 0; l < columns; l++)
		{
			if (islandWithPoint[i][l] == 0)
			{
				length = MinValueWithPoint(distanceMat, i, l, rows, columns);
				return length;
			}
		}
	}
	return length;
}

void MarkCrater(Matrix &mat, Point const &prevPoint, int craterNum, int row, int column)
{
	if (craterNum != 0)
	{
		mat[prevPoint.row][prevPoint.column] = craterNum;
	}
	mat[row][column] = INT_MIN;
}

int GetWayLengthFromCraters(Matrix &distanceMat, int rows, int columns)
{
	int firstLength = INT_MAX;
	int secondLength = INT_MAX;
	int firstCraterNum = 0;
	int secondCraterNum = 0;
	int craterNum = 0;
	Point prevFirstPoint = {INT_MAX, INT_MAX};
	Point prevSecondPoint = {INT_MAX, INT_MAX};
	int nowLength = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int l = 0; l < columns; l++)
		{
			if (distanceMat[i][l] < 0)
			{
				nowLength = MinValueWithPoint(distanceMat, i, l, rows, columns);
				if (distanceMat[i][l] != firstCraterNum && distanceMat[i][l] != secondCraterNum)
				{
					if (firstLength < secondLength && nowLength < secondLength)
					{
						craterNum = secondCraterNum;
						secondCraterNum = distanceMat[i][l];
						if (prevSecondPoint.row != INT_MAX && prevSecondPoint.column != INT_MAX)
						{
							MarkCrater(distanceMat, prevSecondPoint, craterNum, i, l);
						}
						//secondCraterNum = distanceMat[i][l];
						prevSecondPoint.row = i;
						prevSecondPoint.column = l;
					}
					else if (nowLength < firstLength)
					{
						craterNum = firstCraterNum;
						firstCraterNum = distanceMat[i][l];
						if (prevFirstPoint.row != INT_MAX && prevFirstPoint.column != INT_MAX)
						{
							MarkCrater(distanceMat, prevFirstPoint, craterNum, i, l);
						}
						//firstCraterNum = distanceMat[i][l];
						prevFirstPoint.row = i;
						prevFirstPoint.column = l;
					}
				}
				if (distanceMat[i][l] == secondCraterNum && nowLength < secondLength)
				{
					secondLength = nowLength;
					MarkCrater(distanceMat, prevSecondPoint, secondCraterNum, i, l);
					prevSecondPoint.row = i;
					prevSecondPoint.column = l;
				}
				if (distanceMat[i][l] == firstCraterNum && nowLength < firstLength)
				{
					firstLength = nowLength;
					MarkCrater(distanceMat, prevFirstPoint, firstCraterNum, i, l);
					prevFirstPoint.row = i;
					prevFirstPoint.column = l;
				}
			}
		}
	}
	return firstLength + secondLength;
}

Point GetNeighbIndexWithMinElem(Matrix const &mat, Point p, int rows, int columns)
{
	int left = (p.column > 0 && mat[p.row][p.column - 1] >= 0) ? mat[p.row][p.column - 1] : INT_MAX;
	int right = (p.column < columns - 1 && mat[p.row][p.column + 1] >= 0) ? mat[p.row][p.column + 1] : INT_MAX;
	int top = (p.row > 0 && mat[p.row - 1][p.column] >= 0) ? mat[p.row - 1][p.column] : INT_MAX;
	int down = (p.row < rows - 1 && mat[p.row + 1][p.column] >= 0) ? mat[p.row + 1][p.column] : INT_MAX;
	int minInHorizontal = 0;
	Point horizontal;
	horizontal.row = p.row;
	if (left < right) 
	{
		minInHorizontal = left;
		horizontal.column = p.column - 1;
	}
	else
	{
		minInHorizontal = right;
		horizontal.column = p.column + 1;
	}
	int minInVertical = 0;
	Point vertical; 
	vertical.column = p.column;
	if (top < down)
	{
		minInVertical = top;
		vertical.row = p.row - 1;
	}
	else
	{
		minInVertical = down;
		vertical.row = p.row + 1;
	}
	return (minInHorizontal < minInVertical) ? horizontal : vertical;
}

void CreateWay(Matrix const &mat, Point p, int rows, int columns, Matrix &distMat)
{
	distMat[p.row][p.column] = 0;
	while (p.row > 0 && p.column > 0 && p.row < rows - 1 && p.column < columns - 1 && mat[p.row][p.column] != 0)
	{
		p = GetNeighbIndexWithMinElem(mat, p, rows, columns);
		distMat[p.row][p.column] = 0;
	}
}

Matrix MarkVectorFromPoint(Matrix &distanceMat, Matrix &matWithPoint, int rows, int columns)
{
	Point zero = GetPoint(matWithPoint, rows, columns, 0);
	Point firstCrater = GetPoint(matWithPoint, rows, columns, INT_MIN);
	Point secondCrater = GetPoint(matWithPoint, rows, columns, INT_MIN);
	
	firstCrater = GetNeighbIndexWithMinElem(matWithPoint, firstCrater, rows, columns);
	secondCrater = GetNeighbIndexWithMinElem(matWithPoint, secondCrater, rows, columns);

	Matrix mat = distanceMat;
    CreateWay(mat, zero, rows, columns, distanceMat);
	CreateWay(matWithPoint, firstCrater, rows, columns, distanceMat);
	CreateWay(matWithPoint, secondCrater, rows, columns, distanceMat);
	
	return distanceMat;
}

Matrix MarkVectorFromCraters(Matrix &distanceMat, Matrix &mat, int rows, int columns)
{
	Point firstCrater = GetPoint(mat, rows, columns, INT_MIN);
	Point secondCrater = GetPoint(mat, rows, columns, INT_MIN);

	//::cout << firstCrater.row << " " << firstCrater.column << std::endl;
	//std::cout << secondCrater.row << " " << secondCrater.column << std::endl;

	firstCrater = GetNeighbIndexWithMinElem(distanceMat, firstCrater, rows, columns);
	secondCrater = GetNeighbIndexWithMinElem(distanceMat, secondCrater, rows, columns);

	Matrix island = distanceMat;
	CreateWay(island, firstCrater, rows, columns, distanceMat);
	CreateWay(island, secondCrater, rows, columns, distanceMat);

	return distanceMat;
}

Matrix ConnectingCratersWithOcean(std::ifstream &input, int rows, int columns)
{
	// Перенос острова в матрицу.
	// island
	Matrix island = InitiateVector(rows, columns, 0);
	InitiateIsland(input, island, rows, columns);
	
	// Построение матрицы расстояний.
	island = FillInDistances(island, rows, columns);
	Matrix islandWithCraters = island;

	// Поиск точки с кратчайшим путем.
	unsigned int start_time = clock();
	Matrix islandWithPoint = FindPoint(island, rows, columns);
	std::cout << clock() - start_time << std::endl;

	// Подсчет длины путей.
	int lengthFromPoint = CountWayLength(islandWithPoint, island, rows, columns) + GetMaxElement(islandWithPoint, rows, columns);
	int lengthFromCraters = GetWayLengthFromCraters(islandWithCraters, rows, columns);

	//std::cout << lengthFromCraters << std::endl;
	//std::cout << lengthFromPoint << std::endl;
	/*for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
            std::cout << islandWithCraters[i][j] << " ";
		}
		std::cout << std::endl;
	}*/

	// Выбор варианта для печати.
	if (lengthFromPoint < lengthFromCraters)
	{
		// Пометка из точки.
		return MarkVectorFromPoint(island, islandWithPoint, rows, columns);
	}
	else
	{
		// Пометка из кратеров.
		return MarkVectorFromCraters(island, islandWithCraters, rows, columns);
	}
}

int main(int argc, char *argv[])
{
	std::ifstream inputFile;
	std::ofstream outputFile;
	ParametersSeparation(argv, inputFile, outputFile);
	Matrix island;
	int rows = 0;
	int columns = 0;
	if (inputFile.is_open())
	{
		std::string line;
		std::getline(inputFile, line);

		// Получение количества строк и колонок матрицы острова.
		rows = std::stoi(line.substr(0, line.find(" ")));
		columns = std::stoi(line.substr(line.find(" "), line.length()));
		island = ConnectingCratersWithOcean(inputFile, rows, columns);
	}
	else
	{
		std::cerr << "Input file is not exist" << std::endl;
		return 1;
	}
	if (outputFile.is_open())
	{
		PrintWay(outputFile, island, rows, columns);
	}
	return 0;
}