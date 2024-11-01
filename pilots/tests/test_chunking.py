import os
from langchain.text_splitter import RecursiveCharacterTextSplitter
from bs4 import BeautifulSoup

# Load markdown documents from a directory
docs = []
directory = 'C:/Temp/FS/html/'

# Ensure the directory exists and handle file paths correctly
filename = '3DADD_3D_Addendum_Create_Boundary_Trim_Line.htm'
file_path = os.path.join(directory, filename)

# Open the file in read mode and append its content to docs
with open(file_path, 'r', encoding='utf-8') as file:
    docs.append(file.read())

# For each document, extract the text and split it into chunks.
texts = []
for doc in docs:
    soup = BeautifulSoup(doc, 'html.parser')
    text = soup.get_text()
    texts.append(text)

# Initialize the text splitter
text_splitter = RecursiveCharacterTextSplitter(
    chunk_size=1000,  # Maximum size of each chunk
    chunk_overlap=200,  # Overlap between chunks
    separators=["\n\n", "\n", " ", ""],  # Characters to split on
    length_function=len,  # Function to measure chunk size
    is_separator_regex=False  # Whether separators are regex patterns
)

# Split the text into chunks
chunks = text_splitter.create_documents(texts)

# Print the resulting chunks
for i, chunk in enumerate(chunks):
    print(f"Chunk {i+1}: {chunk.page_content}")
