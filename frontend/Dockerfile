# Stage 1: Build the React application
FROM node:20-bookworm-slim AS builder

WORKDIR /app

# Copy package files and install dependencies
COPY package*.json ./
RUN npm ci

# Copy the rest of the application source code
COPY . .

# Pass VITE_API_URL as a build-time argument so Vite can embed it
ARG VITE_API_URL
ENV VITE_API_URL=$VITE_API_URL

# Build the React production bundle
RUN npm run build

# Stage 2: Serve the application using Nginx
FROM nginx:1.25-alpine

# Copy the build output from the builder stage
COPY --from=builder /app/dist /usr/share/nginx/html

# Copy the custom Nginx configuration
COPY nginx.conf /etc/nginx/conf.d/default.conf

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
