
#include <cstdio>
#include <cstdlib>

class TString
{
private:
	char * Mas;
	size_t Size;
	size_t Max;
public:
	TString();
	~TString();
	void PushBack(char toPush);
	void PopFront();
	size_t Length();
	void GetWord();
	void Print();
	char &operator[](int index);
	bool operator==(TString &that);
	void operator=(TString &that);
	void Clear();
};


TString::TString()
{
	Mas = (char *)malloc(sizeof(char) * 32);
	Size = 0;
	Max = 32;
}

TString::~TString()
{
	if (Mas != NULL)
		free(Mas);
}

void TString::PushBack(char toPush)
{
	if ((Max - Size) < 2)
	{
		if (Size == 0)
		{
			Max = 4;
			Mas = (char *)malloc(sizeof(char) * Max);
		}
		else
		{
			char * newMas;
			Max *= 2;
			newMas = (char *)malloc(sizeof(char) * Max);
			for (int i = 0; i != Size; ++i)
				newMas[i] = Mas[i];
			free(Mas);
			Mas = newMas;
		}
	}
	Mas[Size] = toPush;
	if (toPush != '\0')
		++Size;
}

size_t TString::Length()
{
	return Size;
}

void TString::GetWord()
{
	//bool flag = false;
	char c = getchar();
	while (c != ' ' && c != '\n' && c != '\t' && c != EOF)
	{
		//if (c != '0')
		//flag = true;
		//f (flag)
		this->PushBack(c);
		c = getchar();
	}
	//if (c == '0')
	//this->PushBack(c);
	c = '\0';
	this->PushBack(c);
}

void TString::Print()
{
	printf("%s\n", Mas);
}

char &TString::operator[](int index)
{
	if (index >= (int)Size && (int)Size > 0)
		return Mas[Size - 1];
	return (Mas[index]);
}

bool TString::operator==(TString &that)
{
	if (this->Size != that.Size)
		return false;
	else
		for (int i = 0; i != this->Size; ++i)
		{
			if (this->Mas[i] != that.Mas[i])
				return false;
		}
	return true;
}

void TString::operator=(TString &that)
{
	this->Clear();
	for (int i = 0; i != that.Length(); ++i)
		this->PushBack(that[i]);
}

void TString::Clear()
{
	if (Mas != NULL)
		free(Mas);
	Mas = NULL;
	Size = 0;
	Max = 0;
}


class End
{
public:
	int end;
	End(int newEnd)
	{
		this->end = newEnd;
	}
};


class TNode
{
public:


	int First;
	End * Last;
	TNode * SuffLink;
	TNode ** Sons;

	TNode * CreateNode(int start, End * end)
	{
		TNode * newNode = new TNode;
		newNode->Sons = new TNode*[256];
		for (int i = 0; i < 256; ++i)
			newNode->Sons[i] = NULL;
		newNode->First = start;
		newNode->SuffLink = NULL;
		newNode->Last = end;
		return newNode;
	}
};

class TSufferTree
{
private:
	TNode * Root;

	int SuffLeft;

	TNode * ActNode;
	int ActEdge;
	int ActLen;

	End * LeafLast;

	void UkkonenPhase(int i);
	void RPrint(TNode * cur, int depth);
	void DeleteTree();
	void RDeleteTree(TNode * cur);
	int FindLength(TString &needle, int idx);
public:
	TString Origin;
	TSufferTree(TString &source);
	void Print();
	~TSufferTree();
	void StatMatch(TString &needle);
};


void TSufferTree::UkkonenPhase(int i)
{
	SuffLeft++;
	LeafLast->end++;
	TNode * LastCreatedInner = NULL;
	while (SuffLeft > 0)
	{
		if (ActLen == 0)
		{
			if (ActNode->Sons[(int)Origin[i]] == NULL)
			{
				Root->Sons[(int)Origin[i]] = Root->CreateNode(i, this->LeafLast);
				SuffLeft--;
			}
			else
			{
				ActEdge = ActNode->Sons[(int)Origin[i]]->First;
				ActLen++;
				break;
			}
		}
		else
		{
			char c = '\0';
			TNode * temp = ActNode->Sons[(int)Origin[ActEdge]];
			if (temp->Last->end - temp->First >= ActLen &&
				ActNode->Sons[(int)Origin[ActEdge]]->First + ActLen < (int)Origin.Length())
				c = Origin[ActNode->Sons[(int)Origin[ActEdge]]->First + ActLen];
			else if (temp->Last->end - temp->First + 1 == ActLen)
			{
				if (temp->Sons[(int)Origin[i]] != NULL)
					c = Origin[i];
			}
			else
			{
				ActNode = temp;
				ActLen -= temp->Last->end - temp->First + 1;
				ActEdge += temp->Last->end - temp->First + 1;
				if (ActNode->Sons[(int)Origin[ActEdge]]->First + ActLen < (int)Origin.Length())
					c = Origin[ActNode->Sons[(int)Origin[ActEdge]]->First + ActLen];
			}
			if (c != '\0')
			{
				if (c == Origin[i])
				{
					if (LastCreatedInner != NULL) {
						LastCreatedInner->SuffLink = ActNode->Sons[(int)Origin[ActEdge]];
					}

					temp = ActNode->Sons[(int)Origin[ActEdge]];
					if (temp->Last->end - temp->First < ActLen)
					{
						ActNode = temp;
						ActLen -= (temp->Last->end - temp->First);
						ActEdge = temp->First;
					}
					else
						ActLen++;
					break;
				}
				else
				{
					temp = ActNode->Sons[(int)Origin[ActEdge]];
					int oldFirst = temp->First;
					temp->First += ActLen;
					TNode * newInternal = Root->CreateNode(oldFirst, new End(oldFirst + ActLen - 1));
					TNode * newLeaf = Root->CreateNode(i, LeafLast);
					newInternal->Sons[(int)Origin[newInternal->First + ActLen]] = temp;
					newInternal->Sons[(int)Origin[i]] = newLeaf;
					ActNode->Sons[(int)Origin[newInternal->First]] = newInternal;
					if (LastCreatedInner != NULL) {
						LastCreatedInner->SuffLink = newInternal;
					}

					LastCreatedInner = newInternal;
					newInternal->SuffLink = Root;
					if (ActNode != Root)
						ActNode = ActNode->SuffLink;
					else
					{
						ActEdge++;
						ActLen--;
					}
					SuffLeft--;
				}
			}
			else
			{
				TNode * node = ActNode->Sons[(int)Origin[ActEdge]];
				node->Sons[(int)Origin[i]] = node->CreateNode(i, LeafLast);
				if (ActNode != Root)
					ActNode = ActNode->SuffLink;
				else
				{
					ActEdge++;
					ActLen--;
				}
				SuffLeft--;
			}
		}
	}
}

TSufferTree::TSufferTree(TString &source)
{
	Origin = source;
	Origin.PushBack('$');
	Root = Root->CreateNode(1, new End(0));
	ActNode = Root;
	ActEdge = -1;
	ActLen = 0;
	LeafLast = new End(-1);
	for (int i = 0; i < (int)Origin.Length(); ++i)
	{
		UkkonenPhase(i);
	}
}

void TSufferTree::Print()
{
	RPrint(Root, 0);
}

TSufferTree::~TSufferTree()
{
	DeleteTree();
}

void TSufferTree::RPrint(TNode * cur, int depth)
{
	if (cur != NULL)
	{
		for (int i = 0; i < depth; ++i)
			printf("    ");
		if (cur->First != -1)
		{
			for (int i = cur->First; i <= cur->Last->end; ++i)
				printf("%c", Origin[i]);
			printf("\tnode [%d,%d]  ", cur->First, cur->Last->end);
			if (cur->SuffLink != NULL)
				printf("link to [%d,%d]", cur->SuffLink->First, cur->SuffLink->Last->end);
			printf("\n");
		}
		for (int i = 0; i < 256; ++i)
			RPrint(cur->Sons[i], depth + 1);
	}
}

void TSufferTree::DeleteTree()
{
	RDeleteTree(Root);
	delete LeafLast;
}

void TSufferTree::RDeleteTree(TNode * cur)
{
	if (cur != NULL)
	{
		if (cur->Last->end != LeafLast->end)
		{
			delete cur->Last;
			for (int i = 0; i < 256; ++i)
				if (cur->Sons[i] != NULL)
					RDeleteTree(cur->Sons[i]);
		}
		delete cur;
	}
}

void TSufferTree::StatMatch(TString &needle)
{
	int * matches = new int[needle.Length()];
	ActNode = Root;
	ActEdge = -1;
	ActLen = 0;
	for (int i = 0; i < (int)needle.Length(); ++i)
	{
		matches[i] = 0;
		if (ActNode->Last != LeafLast && ActNode->SuffLink != NULL)
		{
			ActNode = ActNode->SuffLink;
			for (int j = i; j < (int)needle.Length(); ++j)
			{
				if (j - i > ActNode->Last->end - ActNode->First)
					break;
				if (needle[j] == Origin[ActNode->First + j - i])
					matches[i]++;
			}
		}
		else
			ActNode = Root;
		ActEdge = -1;
		ActLen = 0;
		matches[i] += FindLength(needle, i + matches[i]);
	}
	for (int i = 0; i < needle.Length(); ++i)
		if (matches[i] == Origin.Length() - 1)
			printf("%d\n", i + 1);
	/*
	for (int i = 0; i < needle.Length(); ++i)
	printf("%d ", matches[i]);
	printf("\n");
	*/
}

int TSufferTree::FindLength(TString &needle, int idx)
{
	int i;
	for (i = idx; i < (int)needle.Length(); ++i)
	{
		if (ActLen == 0)
		{
			if (ActNode->Sons[(int)needle[i]] == NULL)
				break;
			else
			{
				ActEdge = ActNode->Sons[(int)needle[i]]->First;
				ActLen = 1;
			}
		}
		else
		{
			if (ActNode->Sons[(int)Origin[ActEdge]]->Last->end - ActNode->Sons[(int)Origin[ActEdge]]->First >= ActLen && Origin[ActNode->Sons[(int)Origin[ActEdge]]->First + ActLen] == needle[i])
				ActLen++;
			else if (ActNode->Sons[(int)Origin[ActEdge]]->Last->end - ActNode->Sons[(int)Origin[ActEdge]]->First < ActLen)
			{
				ActNode = ActNode->Sons[(int)Origin[ActEdge]];
				if (ActNode->Sons[(int)needle[i]] == NULL)
					break;
				else
				{
					ActEdge = ActNode->Sons[(int)needle[i]]->First;
					ActLen = 1;
				}
			}
			else
				break;
		}
	}
	return i - idx;
}

int main()
{
	TString text, needle;
	text.GetWord();
	TSufferTree suffixTree(text);
	suffixTree.Print();
	needle.GetWord();
	suffixTree.StatMatch(needle);
	getchar();
	return 0;
}
