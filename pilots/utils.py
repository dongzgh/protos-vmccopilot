from bs4 import BeautifulSoup
import torch
from langchain.text_splitter import RecursiveCharacterTextSplitter
import faiss
from langchain_community.vectorstores import FAISS
from langchain_community.docstore.in_memory import InMemoryDocstore
from sentence_transformers import SentenceTransformer
from langchain_huggingface import HuggingFaceEmbeddings
import os
from os.path import realpath, join
import shutil
os.environ['KMP_DUPLICATE_LIB_OK'] = 'True'


def extract_text(fn: str) -> str:
    with open(fn, 'r') as f:
        doc = f.read()
    soup = BeautifulSoup(doc, 'html.parser')
    text = soup.get_text()

    # Remove extra spaces and newlines.
    text = ' '.join(text.split())
    return text.strip()


def chunk_docs(docs: list) -> list:
    # Initialize the text splitter
    text_splitter = RecursiveCharacterTextSplitter(
        chunk_size=1000,  # Maximum size of each chunk
        chunk_overlap=200,  # Overlap between chunks
        separators=["\n\n", "\n", " ", ""],  # Characters to split on
        length_function=len,  # Function to measure chunk size
        is_separator_regex=False  # Whether separators are regex patterns
    )

    # Split the text into chunks
    chunks = text_splitter.create_documents(docs)
    return chunks


def save_embeddings(chunks: list, db_dir: str):
    # Initialize embedding model
    embeddings = HuggingFaceEmbeddings(
        model_name='sentence-transformers/all-MiniLM-L6-v2')

    # Initialize FAISS index
    dimension = len(embeddings.embed_query(chunks[0].page_content))
    index = faiss.IndexFlatL2(dimension)

    # Create a FAISS vector store
    vector_store = FAISS(
        embedding_function=embeddings,
        index=index,
        docstore=InMemoryDocstore(),
        index_to_docstore_id={},
    )

    # Add embeddings and documents to the vector store
    size = len(chunks)
    for i, chunk in enumerate(chunks):
        print(f'Adding chunk {i+1} of {size} to the vector store...')
        vector_store.add_documents([chunk])

    # Save the FAISS index to disk
    if os.path.exists(db_dir):
        shutil.rmtree(db_dir)
    vector_store.save_local(db_dir)


def search_text(text: str, db_dir: str, k: int = 5) -> list:   
    # Initialize embedding model
    embeddings = HuggingFaceEmbeddings(model_name='sentence-transformers/all-MiniLM-L6-v2')

    # Load the FAISS index.
    vector_store = FAISS.load_local(db_dir, embeddings, allow_dangerous_deserialization=True)

    # Search query.
    results = vector_store.similarity_search(text, k)
    return results
