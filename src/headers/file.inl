// file.inl

void File::_setFilePath(std::string sFilePath)
{
	this -> _sPath = sFilePath;

	std::vector<std::string> pathParts = zer::athm::split(zer::athm::replace(this -> _sPath, "\\", "/"), "/");
	this -> _sFullName = pathParts[pathParts.size() - 1];

	std::vector<std::string> fileNameParts = zer::athm::split(this -> _sFullName, ".");
	
	this -> _sName = fileNameParts[0];
	this -> _sFormat = fileNameParts[1];
}

void File::open(std::string sFilePath)
{
	this -> _setFilePath(sFilePath);
	this -> _bDoesExists = this -> _doesExists(sFilePath);
	
	if (!this -> _bDoesExists && this -> _bWarnings)
			mWARNING("file \"" << this -> _sPath << "\" does not exists");
	else
	{
		this -> _iLength = std::ifstream(this -> _sPath, std::ifstream::ate).tellg();
	}
}

bool File::_bHasModifier(std::initializer_list<int> modifiers, int iDesiredModifier)
{
	for (int iModifier : modifiers)
		if (iModifier == iDesiredModifier)
			return true;
	return false;
}

bool File::_doesExists(std::string sFilePath)
{
	struct stat buffer;
	return stat(sFilePath.c_str(), &buffer) == 0;
}

void File::write(std::string sData, std::string sFilePath, std::initializer_list<int> modifiers)
{
	this -> _setFilePath(sFilePath);
	this -> write(sData, modifiers);
}

void File::write(std::string sData, std::initializer_list<int> modifiers)
{
    #ifdef _WIN32
        if (this -> _bHasModifier(modifiers, file::Modifier::binary))
            this -> _fs.open(this -> _sPath, std::fstream::out | std::ios::binary);
        else
            this -> _fs.open(this -> _sPath, std::fstream::out);

        if (this -> _bHasModifier(modifiers, file::Modifier::hidden))
        {
            /*
                Make the file hidden.
            */
            LPCSTR fileLPCSTR = this -> _sPath.c_str();
            DWORD dw = GetFileAttributes(fileLPCSTR);
            if ((dw & FILE_ATTRIBUTE_HIDDEN) == 0)
                SetFileAttributes(fileLPCSTR, dw | FILE_ATTRIBUTE_HIDDEN);
        }
    #endif

	this -> _write(sData);
}

void File::_write(std::string sData)
{
	this -> _fs << sData;
	this -> _sData = sData;
	this -> _bDoesExists = true;
}

void File::read(std::string sFilePath, std::initializer_list<int> modifiers)
{
	this -> open(sFilePath);
	this -> read(modifiers);
}

void File::read(std::initializer_list<int> modifiers)
{
	if (this -> _bDoesExists)
	{
		if (this -> _bHasModifier(modifiers, file::Modifier::binary))
			this -> _fs.open(this -> _sPath, std::fstream::in | std::ios::binary);
		else
			this -> _fs.open(this -> _sPath, std::fstream::in);

		if (this -> _bHasModifier(modifiers, file::Modifier::lines))
			this -> _readLines();
		else
			this -> _read();
	}
	else
		mWARNING("no open file found");
}

void File::_read()
{
	std::string sData((std::istreambuf_iterator<char>(this -> _fs)), std::istreambuf_iterator<char>());
	this -> _sData = sData;
}

void File::_readLines()
{
	std::string sLine;
	while (std::getline(this -> _fs, sLine))
		this -> _lines.push_back(sLine);

	for (int i = 0; i < this -> _lines.size(); ++i)
		this -> _sData += this -> _lines[i] + (i < this -> _lines.size() - 1 ? "\n" : "");
}

void File::setSliceSize(int iSliceSize)
{
	this -> _iSliceSize = iSliceSize;
	if (this -> _bDoesExists)
		this -> _iSlicesLen = std::ceil((float)this -> _iLength / this -> _iSliceSize);
	else
		mWARNING("can't set slice size, because file not open");
}

void File::slice(int iSliceIndex, std::initializer_list<int> modifiers)
{
	if (this -> _bDoesExists)
	{
		if (this -> _bHasModifier(modifiers, file::Modifier::binary))
			this -> _fs.open(this -> _sPath, std::ifstream::ate | std::fstream::in | std::ios::binary);
		else
			this -> _fs.open(this -> _sPath, std::ifstream::ate | std::fstream::in);
		this -> _slice(iSliceIndex);
	}
	else
		mWARNING("no open file found");
}

void File::_slice(int iSliceIndex)
{
	if (iSliceIndex >= 0 && iSliceIndex < this -> _iSlicesLen)
	{
		int iStartPosition = iSliceIndex * this -> _iSliceSize;
		int iEndPosition = iSliceIndex * this -> _iSliceSize + this -> _iSliceSize;

		if (iEndPosition > this -> _iLength)
			iEndPosition -= iEndPosition - this -> _iLength;

		int iTempSliceSize = iEndPosition - iStartPosition;
		
		this -> _sData.resize(iTempSliceSize);

		this -> _fs.seekg(iStartPosition);
		this -> _fs.read(&this -> _sData[0], iTempSliceSize);
	}
	else
		mWARNING("incorrect slice number: " << iSliceIndex << ", with total: " << this -> _iSlicesLen - 1 << 
			" (count slices from zero: 0, 1, 2, etc.)");
}