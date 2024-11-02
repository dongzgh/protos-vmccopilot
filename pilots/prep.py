from os import listdir
from os.path import realpath, join
from utils import extract_text, chunk_docs, save_embeddings

# Define HTML document path.
doc_dir = 'C:/Temp/FS/html/'

# Extract text from the document.
docs = []
for name in listdir(doc_dir):
    fn = realpath(join(doc_dir, name))

    # Extract text from the document.
    print(f'Extracting text from {fn}...')
    text = extract_text(fn)
    docs.append(text)

# Chunk text by sentences.
print('Chunking texts...')
chunks = chunk_docs(docs)

# Embed and save chunks to database.
print('Embedding and saving chunks...')
db_dir = realpath('./pilots/db')
save_embeddings(chunks, db_dir)
